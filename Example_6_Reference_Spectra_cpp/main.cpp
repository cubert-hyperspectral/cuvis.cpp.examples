#include "cuvis.hpp"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    {
        /*
        * Reprocess a measurement with reference spectra
        *
        * Cuvis 3.6 adds two spectrum reference types to the *ProcessingContext*:
        *   - *TargetSpectrum*: Reflectance of the white target (fractions, 1.0 = 100%). Default without it: 100% at every wavelength, or the white_target_reflectivity_percentage setting.
        *   - *WhiteSpectrum*: Averaged white spectrum in dark-corrected sensor counts. Replaces the White reference measurement.
        *
        * The measurement is processed to reflectance with a target spectrum, then with a white spectrum.
        * Each result is exported as a SessionFile including the cube.
        *
        * Used principles:
        *   - *SessionFile* to load a recorded measurement and its references
        *   - *ProcessingContext* with `set_reference_target_spectrum` and `set_reference_white_spectrum`
        *   - *CubeExporter* to save the results
        *
        * Prerequisites to running this example:
        *   - Have a recorded measurement in *SessionFile* format (.cu3s) that contains Dark and White references *or* downloaded the provided [demo data](https://cloud.cubert-gmbh.de/s/SDKSampleData)
        *   - Have the Cuvis SDK 3.6 or newer installed
        *
        *   Run properties
        *   - "path/to/settings" "path/to/measurement/single.cu3s" "OutputFolderName"
        */
        if (argc != 4)
        {
            std::cout << std::endl << "Too few Arguments! Please provide:" << std::endl;
            std::cout << "user settings directory" << std::endl;
            std::cout << "measurement file with Dark and White references (.cu3s)" << std::endl;
            std::cout << "Name of output directory" << std::endl;

            return -1;
        }

        char* const userSettingsDir = argv[1];
        char* const measurementLoc = argv[2];
        char* const outDir = argv[3];

        std::cout << "Example 06 reference spectra cpp " << std::endl;
        std::cout << "User Settings Dir: " << userSettingsDir << std::endl;
        std::cout << "measurement file (.cu3s): " << measurementLoc << std::endl;
        std::cout << "output Dir: " << outDir << std::endl;

        std::cout << "loading settings... " << std::endl;
        cuvis::General::init(userSettingsDir);
        cuvis::General::set_log_level(loglevel_info);

        // The ProcessingContext loads the Dark and White references stored in the SessionFile
        std::cout << "Loading processing context" << std::endl;
        cuvis::SessionFile sess(measurementLoc);
        cuvis::ProcessingContext proc(sess);
        cuvis::ProcessingArgs procArgs;
        procArgs.processing_mode = cuvis::processing_mode_t::Cube_Reflectance;
        proc.set_processingArgs(procArgs);

        cuvis::SaveArgs saveArgs;
        saveArgs.allow_overwrite = true;
        saveArgs.allow_info_file = false;
        saveArgs.full_export = true; // include the reflectance cube in the export

        /*
        * Process a fresh Measurement from the SessionFile with the current references and export it.
        */
        auto process_and_export = [&](std::string const& name) {
            auto optMesu = sess.get_mesu(0);
            assert(optMesu.has_value());
            cuvis::Measurement mesu = optMesu.value();
            std::cout << "processing to reflectance with " << name << std::endl;
            proc.apply(mesu);
            saveArgs.export_dir = std::filesystem::path(outDir) / name;
            cuvis::CubeExporter exporter(saveArgs);
            exporter.apply(mesu);
        };

        // Dark Subtract cube of the White reference measurement: source of the white spectrum below
        auto optWhite = proc.get_reference(cuvis::reference_type_t::Reference_White);
        assert(optWhite.has_value());
        cuvis::Measurement white = optWhite.value();

        cuvis::ProcessingContext darkSubProc(sess);
        cuvis::ProcessingArgs darkSubArgs;
        darkSubArgs.processing_mode = cuvis::processing_mode_t::Cube_DarkSubtract;
        darkSubProc.set_processingArgs(darkSubArgs);
        darkSubProc.apply(white);
        auto const& whiteCube = std::get<cuvis::image_t<std::uint16_t>>(white.get_imdata()->at(CUVIS_MESU_CUBE_KEY));

        /*
        * Target Spectrum
        *   - Values are fractions (1.0 = 100%), wavelengths in nm.
        *   - The spectrum must cover the cube's entire wavelength range, otherwise processing fails.
        *   - The SDK interpolates linearly between the points; they do not need to match the cube's wavelengths.
        *   - For a constant reflectivity, e.g. 95% (the constant reflectivity option of CuvisNEXT),
        *     set two points spanning the cube's range: the cube's first and last wavelength, values {0.95, 0.95}.
        *     Alternatively, `white_target_reflectivity_percentage` in cuvis.settings sets it for all processing (default 100);
        *     an explicitly set target spectrum overrides it.
        */
        // Example certificate of a white target (illustrative values): wavelength [nm] and reflectance as fraction
        cuvis::target_spectrum_t target;
        target.wavelengths = {300, 350, 400, 450, 500, 600, 700, 800, 900, 1000, 1100};
        target.values = {0.80f, 0.86f, 0.91f, 0.94f, 0.95f, 0.96f, 0.96f, 0.96f, 0.95f, 0.94f, 0.93f};
        proc.set_reference_target_spectrum(target);

        process_and_export("TargetSpectrum");

        proc.clear_reference(cuvis::reference_type_t::Reference_TargetSpectrum);

        /*
        * White Spectrum
        *   - Dark-corrected sensor counts (uint16), wavelengths in nm, covering the cube's entire wavelength range.
        *   - `effective_bit_depth` (1 to 16) is required: the bit depth of the sensor's pixel format, e.g. 12 for Mono12Packed.
        *   - `integration_time` [ms]: not used by default (`correct_ref_integration_time="false"` in cuvis.settings),
        *     so record the white spectrum with the same integration time as the measurements.
        *     With "true", the reflectance is scaled by white spectrum integration time / measurement integration time.
        *   - Channels with 0 counts (demo data: 350-358 nm) give no meaningful reflectance:
        *     setting such a spectrum logs a warning, and processing flags the cube as white-dark underflow.
        *   - White spectrum and White reference measurement are mutually exclusive: setting one clears the other.
        *
        * Limitation: A white spectrum is applied to the whole image equally and does not correct spatial effects,
        * like lens vignetting or uneven illumination, as a White reference measurement does.
        * This is an intended limitation of the current implementation, the first iteration of this feature:
        * some cameras allow exchanging the objective, which makes a fixed, calibrated spatial correction invalid in some cases.
        * For spatially accurate reflectance across the whole image, use a White reference measurement.
        */
        // Average the dark-corrected counts over a 40 x 40 pixel region in the image center
        std::size_t const cx = whiteCube._width / 2;
        std::size_t const cy = whiteCube._height / 2;
        std::vector<double> whiteCounts(whiteCube._channels, 0.0);
        for (std::size_t y = cy - 20; y < cy + 20; y++)
            for (std::size_t x = cx - 20; x < cx + 20; x++)
                for (std::size_t chn = 0; chn < whiteCube._channels; chn++)
                    whiteCounts[chn] += whiteCube.get(x, y, chn) / 1600.0;

        cuvis::sensor_spectrum_t whiteSpectrum;
        for (std::size_t chn = 0; chn < whiteCube._channels; chn++)
        {
            whiteSpectrum.wavelengths.push_back(static_cast<float>(whiteCube._wavelength[chn]));
            whiteSpectrum.values.push_back(static_cast<std::uint16_t>(std::nearbyint(whiteCounts[chn])));
        }

        std::string const pixelFormat = white.get_sensor_info()->at("IMAGE_info").pixel_format;
        whiteSpectrum.effective_bit_depth =
            static_cast<std::uint16_t>(std::stoi(pixelFormat.substr(pixelFormat.find_first_of("0123456789"))));
        whiteSpectrum.integration_time = white.get_meta()->integration_time;
        proc.set_reference_white_spectrum(whiteSpectrum);

        process_and_export("WhiteSpectrum");
    }
  cuvis::General::shutdown();
  std::cout << "finished." << std::endl;
  return 0;
}
