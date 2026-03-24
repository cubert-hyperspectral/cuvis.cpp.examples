#include "cuvis.hpp"

#include <cassert>
#include <iostream>

//#include <opencv/opencv.hpp>

int main(int argc, char* argv[])
{
    {
        /*
        * Load and analyse a recorded measurement
        *
        * In this example an already recorded measurement (SessionFile .cu3s) is loaded.
        * The measurement's data and meta-data are accessed.
        *
        * Used principles:
        *   - *SessionFile* to load a recorded measurement
        *   - *Measurement* to access the SessionFiles data and meta-data
        *
        * Prerequisites to running this example:
        *   - Have a recorded measurement in *SessionFile* format (.cu3s) *or* downloaded the provided [demo data](https://drive.google.com/drive/folders/1Cjb0v_a2p1cCmhKH8w2OuRtnhXCJGz61?usp=sharing)
        *   - Have the Cuvis SDK installed
        *
        * Run properties
        *   - "path/to/settings" "path/to/file/file.cu3s"
        */
        if (argc != 3)
        {
            std::cout << "Too few Arguments! Please provide:" << std::endl;
            std::cout << "user settings directory" << std::endl;
            std::cout << "sessionfile (.cu3s)" << std::endl;

            return -1;
        }

        char* const userSettingsDir = argv[1];
        char* const sessionLoc = argv[2];

        std::cout << "Example 01 load measurement cpp " << std::endl;
        std::cout << "User Settings Dir: " << userSettingsDir << std::endl;
        std::cout << "sessionfile (.cu3s): " << sessionLoc << std::endl;

        /*
        * **Settings**
        * Initialize the Cuvis SDK using a settings - directory
        * This is optional(all settings have defaults),
        * but enables you to optimize Cuvis' performance on your system using the settings
        * Your camera and the default Cuvis installation both provide these settings files
        */
        std::cout << "loading user settings..." << std::endl;
        cuvis::General::init(userSettingsDir);
        cuvis::General::set_log_level(loglevel_info);

        /*
        * **SessionFile**
        * A SessionFile is a Cubert-proprietary container file format for storing measurement data from Cubert cameras.
        * It simplifies dealing with the calibration files, reference measurement and actual measurements by merging everything into a single file.
        *
        * A SessionFile can contain:
        *   - One or more *Measurements*
        *   - Reference Measurements (Dark, White, Distance, ...) (normally one per type)
        *   - Camera calibration file and Spectral Radiance calibration file
        *   - Meta-data about the recording settings (frame-rate, session name, etc.)
        */
        std::cout << "loading session... " << std::endl;
        cuvis::SessionFile sess(sessionLoc);
        /*
        * **Measurement**
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
        auto optmesu = sess.get_mesu(0);
        assert(optmesu.has_value());
        cuvis::Measurement mesu = optmesu.value();

        std::cout << "Data 1" << mesu.get_meta()->name << " "
            << "t=" << mesu.get_meta()->integration_time << " ms "
            << "mode=" << mesu.get_meta()->processing_mode << " " << std::endl;
        if (mesu.get_meta()->measurement_flags.size() > 0)
        {
            std::cout << "  Flags" << std::endl;
            for (auto const& flags : mesu.get_meta()->measurement_flags)
            {
                std::cout << "  - " << flags.first << " (" << flags.second << ")"
                    << std::endl;
            }
        }

        assert(
            mesu.get_meta()->processing_mode == Cube_Raw &&
            "This example requires raw mode");

        auto const& cube_it = mesu.get_imdata()->find(CUVIS_MESU_CUBE_KEY);
        assert(cube_it != mesu.get_imdata()->end() && "Cube not found");

        auto cube = std::get<cuvis::image_t<std::uint16_t>>(cube_it->second);

        //uncomment to show a single channel with openCV
        /*
          cv::Mat img(
          cv::Size(cube._width, cube._height),
          CV_16UC(cube._channels),
          const_cast<void*>(reinterpret_cast<const void*>(cube._data)),
          cv::Mat::AUTO_STEP);

          cv::Mat singleChannel;
          cv::extractChannel(
              img, singleChannel, 25); // extract channel 25 as an example
          singleChannel.convertTo(singleChannel, CV_8U, 1 / 16.0);
          cv::imshow("channel 25", singleChannel);
          cv::waitKey(0);
          */

        std::size_t x = 120;
        std::size_t y = 200;

        assert(x < cube._width && "x index exceeds cube width");
        assert(y < cube._height && "y index exceeds cube height");

        std::cout << "lambda [nm]; raw counts [au] " << std::endl;

        for (std::size_t chn = 0; chn < cube._channels; chn++)
        {
            // memory layout:
            //unsigned index = (y * cube.width + x) * cube.channels + chn;
            //uint16_t value = cube16bit[index];

            auto const value = cube.get(x, y, chn);
            unsigned lambda = cube._wavelength[chn];

            std::cout << lambda << "; " << value << std::endl;
        }
    }
  cuvis::General::shutdown();
  std::cout << "finished. " << std::endl;
  return 0;
}
