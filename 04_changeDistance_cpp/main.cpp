#include "cuvis.hpp"

#include <cassert>
#include <iostream>

int main(int argc, char* argv[])
{
    {
        /*
        * Load and change a measurement
        *
        * In this example a measurement is loaded and the distance is changed.
        *
        * * Used principles:
        *   - *SessionFile* to load a recorded measurement
        *
        * Run properties
        *   - "path/to/settings" path/to/sesstion/file.cu3s" distanceInMm "OutputFolderName"
        */
        if (argc != 5)
        {
            std::cout << std::endl << "Too few Arguments! Please provide:" << std::endl;
            std::cout << "user settings directory" << std::endl;
            std::cout << "sessionfile (.cu3s)" << std::endl;
            std::cout << "new distance in mm" << std::endl;
            std::cout << "Name of export directory" << std::endl;

            return -1;
        }

        char* const userSettingsDir = argv[1];
        char* const sessionLoc = argv[2];
        char* const distanceString = argv[3];
        char* const exportDir = argv[4];

        int distance = std::stoi(distanceString);

        std::cout << "Example 04 change distance cpp " << std::endl;
        std::cout << "User Settings Dir: " << userSettingsDir << std::endl;
        std::cout << "sessionfile (.cu3s): " << sessionLoc << std::endl;
        std::cout << "New Distance in mm: " << distance << std::endl;
        std::cout << "Export Dir: " << exportDir << std::endl;

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

        std::cout << "loading session... " << std::endl;
        cuvis::SessionFile sess(sessionLoc);

        std::cout << "loading measurement... " << std::endl;
        auto optmesu = sess.get_mesu(0);
        assert(optmesu.has_value());
        cuvis::Measurement mesu = optmesu.value();

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
        std::cout << "Loading Calibration and processing context..." << std::endl;
        cuvis::ProcessingContext proc(sess);

        cuvis::SaveArgs saveArgs;
        saveArgs.allow_overwrite = true;
        saveArgs.export_dir = exportDir;
        saveArgs.allow_session_file = true;

        cuvis::CubeExporter exporter(saveArgs);

        std::cout << "setting distance..." << std::endl;
        proc.calc_distance(distance);

        cuvis::ProcessingArgs procArgs = proc.get_processingArgs();
        procArgs.processing_mode = cuvis::processing_mode_t::Cube_Raw;

        assert(proc.is_capable(mesu, procArgs));

        std::cout << "changing distance..." << std::endl;
        proc.apply(mesu);
        std::cout << "saving..." << std::endl;
        exporter.apply(mesu);
    }
  cuvis::General::shutdown();
  std::cout << "finished." << std::endl;
  return 0;
}
