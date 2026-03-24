#include "cuvis.hpp"

#include <cassert>
#include <iostream>

int main(int argc, char* argv[])
{
    {
        /*
        *  Load and reprocess a recorded measurement
        *
        *
        * In this example an already recorded measurement (SessionFile .cu3s) is loaded.
        * The measurement is reprocessed into different processing modes, explaining their differences.
        *
        * Used principles:
        *   - *SessionFile* to load a recorded measurement
        *   - *Measurement* to access the SessionFiles data and meta-data
        *   - *ProcessingContext* to generate hyperspectral cubes using different processing modes
        *
        * Prerequisites to running this example:
        *   - Have a recorded measurement in *SessionFile* format (.cu3s) *or* downloaded the provided [demo data](https://drive.google.com/drive/folders/1Cjb0v_a2p1cCmhKH8w2OuRtnhXCJGz61?usp=sharing)
        *   - Have a recorded White and Dark reference measurement *or* use the [demo data](https://drive.google.com/drive/folders/1Cjb0v_a2p1cCmhKH8w2OuRtnhXCJGz61?usp=sharing)
        *   - Have the Cuvis SDK installed
        *
        *   Run properties
        *   - "path/to/settings" path/to/measurement/single.cu3s" "path/to/dark/file.cu3s" "path/to/white/file.cu3s" "path/to/distance/file.cu3s" "OutputFolderName"
        */
        if (argc != 7)
        {
            std::cout << std::endl << "Too few Arguments! Please provide:" << std::endl;
            std::cout << "user settings directory" << std::endl;
            std::cout << "measurement file (.cu3s)" << std::endl;
            std::cout << "dark file (.cu3s)" << std::endl;
            std::cout << "white file (.cu3s)" << std::endl;
            std::cout << "distance file (.cu3s)" << std::endl;
            std::cout << "Name of output directory" << std::endl;

            return -1;
        }

        char* const userSettingsDir = argv[1];
        char* const measurementLoc = argv[2];
        char* const darkLoc = argv[3];
        char* const whiteLoc = argv[4];
        char* const distanceLoc = argv[5];
        char* const outDir = argv[6];

        std::cout << "Example 02 reprocess measurement cpp " << std::endl;
        std::cout << "User Settings Dir: " << userSettingsDir << std::endl;
        std::cout << "measurement file (.cu3s): " << measurementLoc << std::endl;
        std::cout << "dark file (.cu3s): " << darkLoc << std::endl;
        std::cout << "white file (.cu3s): " << whiteLoc << std::endl;
        std::cout << "distance file (.cu3s): " << distanceLoc << std::endl;
        std::cout << "output Dir: " << outDir << std::endl;

        /*
        * Settings
        * Initialize the Cuvis SDK using a settings - directory
        * This is optional(all settings have defaults),
        * but enables you to optimize Cuvis' performance on your system using the settings
        * Your camera and the default Cuvis installation both provide these settings files
        */
        std::cout << "loading settings... " << std::endl;
        cuvis::General::init(userSettingsDir);
        cuvis::General::set_log_level(loglevel_info);

        /*
        * Measurement
        * The *Measurement* class is the storage container for the actual hyperspectral data, along with more specific meta-data - things that can change from measurement to measurement (eg. integration time).
        *
        *  A Measurement can contain:
        *   - Multiple image data
        *   - A hyperspectral data cube
        *   - "Links" to reference measurements
        *   - Recording settings
        *   - Meta-data about the state of the camera
        *   - Meta-data about the software used to capture the measurement
        *   - Meta-data about the quality of the measurement
        *
        * Please note:
        * The hyperspectral cube is not always present.
        * Processing the measurement to generate the hyperspectral cube is a compute-intensive step and is thus only done on-demand!
        * By default, measurements are stored without the cube, but with all necessary data to generate it, to speed up saving and save on disk space (this shrinks measurements on average by about 50%)!
        */
        std::cout << "loading measurement... " << std::endl;
        cuvis::SessionFile sessMesu(measurementLoc);
        auto optMesu = sessMesu.get_mesu(0);
        assert(optMesu.has_value());
        cuvis::Measurement mesu = optMesu.value();

        std::cout << "loading dark... " << std::endl;
        cuvis::SessionFile sessDark(darkLoc);
        auto optDark = sessDark.get_mesu(0);
        assert(optDark.has_value());
        cuvis::Measurement dark = optDark.value();

        std::cout << "loading white... " << std::endl;
        cuvis::SessionFile sessWhite(whiteLoc);
        auto optWhite = sessWhite.get_mesu(0);
        assert(optWhite.has_value());
        cuvis::Measurement white = optWhite.value();

        /*
        * Distance Calibration
        * Most Ultris cameras(except for Relay - variants) require distance calibration to achieve optimal results.
        *
        * ** Please note : **The provided default demo dataset was recorded with a relay - equipped camera(Ultris XM with relay optics).Thus this step is not applicable to this dataset.
        *
        * Distance calibration is an operation that can be done with already recorded data and requires a distance reference measurement.
        * The reference should contain high - contrast data over the relevant spectral channels at the desired distance that data should be calibrated to.
        *
        * In this example, the measurement itself will be used as the distance reference.If the target object is suitable(high contrast, non - repeating patterns), this can suffice for good results.
        */
        std::cout << "loading distance... " << std::endl;
        cuvis::SessionFile sessDistance(distanceLoc);
        auto optDistance = sessDistance.get_mesu(0);
        assert(optDistance.has_value());
        cuvis::Measurement distance = optDistance.value();

        std::cout << "Data 1" << mesu.get_meta()->name << " "
            << "t=" << mesu.get_meta()->integration_time << " ms "
            << "mode=" << mesu.get_meta()->processing_mode << " " << std::endl;

        /*
        * Processing Context
        * The* ProcessingContext* is the interface that enables computing a hyperspectral cube from a measurement.
        * A camera calibration file is required to initialize the* ProcessingContext*, as each Cubert camera is individually calibrated to provide the most accurate spectral information.
        * As a SessionFile contains the camera calibration, it is used to construct the* ProcessingContext* .
        *
        * To generate a hyperspectral cube, the* ProcessingContext* is** applied** to the* Measurement* .The* Measurement* is modified** in - place * *and now contains a cube.
        *
        * To select the processing mode, write the `processing_mode` attribute.
        *
        * When initializing a* ProcessingContext* from a* SessionFile*, the reference* Measurements* stored in the* SessionFile* are automatically loaded and set within the* ProcessingContext* .
        * Using the method `set_reference`, different measurements can be set for each reference type.
        */
        std::cout << "Loading processing context" << std::endl;
        cuvis::ProcessingContext proc(sessMesu);

        std::cout << "Set references" << std::endl;

        proc.set_reference(dark, cuvis::reference_type_t::Reference_Dark);
        proc.set_reference(white, cuvis::reference_type_t::Reference_White);
        proc.set_reference(distance, cuvis::reference_type_t::Reference_Distance);

        cuvis::ProcessingArgs procArgs;
        cuvis::SaveArgs saveArgs;
        saveArgs.allow_overwrite = true;
        saveArgs.allow_session_file = true;
        saveArgs.allow_info_file = false;

        std::map<std::string, cuvis::processing_mode_t> target_modes = {
            {"Raw", cuvis::processing_mode_t::Cube_Raw},
            {"DS", cuvis::processing_mode_t::Cube_DarkSubtract},
            {"Ref", cuvis::processing_mode_t::Cube_Reflectance},
            {"RAD", cuvis::processing_mode_t::Cube_SpectralRadiance} };

        for (auto const& mode : target_modes)
        {
            procArgs.processing_mode = mode.second;
            if (proc.is_capable(mesu, procArgs))
            {
                std::cout << "processing to mode " << mode.first << std::endl;
                proc.set_processingArgs(procArgs);
                proc.apply(mesu);
                saveArgs.export_dir = std::filesystem::path(outDir) / mode.first;

                cuvis::CubeExporter exporter(saveArgs);
                exporter.apply(mesu);
            }
            else
            {
                std::cout << "cannot process to mode " << mode.first << std::endl;
            }
        }
    }
  cuvis::General::shutdown();
  std::cout << "finished." << std::endl;
  return 0;
}
