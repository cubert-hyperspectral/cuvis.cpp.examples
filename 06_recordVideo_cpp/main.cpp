#include "cuvis.hpp"

#include <cassert>
#include <chrono>
#include <cmath>
#include <csignal>
#include <ctime>
#include <iostream>
#include <filesystem>

using namespace std::literals::chrono_literals;
namespace fs = std::filesystem;

int keepRunning = 1;

void signal_handler(int sig)
{
  (void)sig;
  std::cout << std::endl
            << "CTRL+C received. Stopping acquisiton..." << std::endl;
  keepRunning = 0;
}


int main(int argc, char* argv[])
{
  if (argc != 7)
  {
    std::cout << std::endl << "Too few Arguments! Please provide:" << std::endl;
    std::cout << "user settings directory  or .cu3c file" << std::endl;
    std::cout << "factory directory" << std::endl;
    std::cout << "name of recording directory" << std::endl;
    std::cout << "exposure time in ms" << std::endl;
    std::cout << "auto exposure [1/0]" << std::endl;
    std::cout << "target fps" << std::endl;

    return -1;
  }

  char* userSettingsDir = argv[1];
  fs::path factoryDir(argv[2]);
  char* recDir = argv[3];
  char* exposureString = argv[4];
  char* autoExpString = argv[5];
  char* fpsString = argv[6];

  int exposure_ms = std::stoi(exposureString);
  bool autoExp = std::stoi(autoExpString);
  double fps = std::stod(fpsString);

  std::cout << "Example 06 video cpp " << std::endl;
  std::cout << "User Settings Dir: " << userSettingsDir << std::endl;
  std::cout << "Factory Dir or .cu3c file: " << factoryDir.string() << std::endl;
  std::cout << "Recording Dir: " << recDir << std::endl;
  std::cout << "Exposure in ms: " << exposure_ms << std::endl;
  std::cout << "Auto Exposure: " << autoExp << std::endl;
  std::cout << "Target FPS: " << fps << std::endl;

  std::cout << "loading user settings..." << std::endl;

  cuvis::General::init(userSettingsDir);

  //register log message output
  cuvis::General::register_log_callback(
      [](char const* msg, cuvis::loglevel_t lvl) -> void {
        static std::map<cuvis::loglevel_t, std::string> log_prefix = {
            {loglevel_info, "info: "},
            {loglevel_warning, "warning: "},
            {loglevel_error, "error: "},
            {loglevel_fatal, "fatal: "}};

        std::cout << " - " << log_prefix.at(lvl) << msg << std::endl;
      },
      loglevel_info);

  std::cout << "loading calibration..." << std::endl;
  const auto  get_calib = [factoryDir]()
      {
          if (fs::is_directory(factoryDir))
          {
              return  cuvis::Calibration(factoryDir);
          }
          else if (fs::is_regular_file(factoryDir) && factoryDir.extension() == ".cu3c") {
              std::cout << " using .cu3c file as calibration instead of factory dir..." << std::endl;
              cuvis::SessionFile calibFile(factoryDir);
              return cuvis::Calibration(calibFile);
          }
          else {
              throw std::invalid_argument("Unrecognized file format.");
          }
      };

  cuvis::Calibration calib = get_calib();

  std::cout << "loading acquisition context..." << std::endl;
  cuvis::AcquisitionContext acq(calib);
  CUVIS_SESSION_INFO sess = {"video", 0, 0};
  acq.set_session_info(sess);

  std::cout << "prepare saving of measurements..." << std::endl;
  cuvis::SaveArgs sargs;
  sargs.fps = fps;
  sargs.operation_mode = OperationMode_Internal;
  sargs.allow_overwrite = true;
  sargs.allow_session_file = true;
  sargs.export_dir = std::filesystem::path(recDir);

  std::cout << "Writing Files to: " << sargs.export_dir << std::endl;
  cuvis::CubeExporter exporter(sargs);

  std::cout << "prepare processing of measurements..." << std::endl;
  cuvis::ProcessingContext proc(calib);
  cuvis::ProcessingArgs args;
  args.processing_mode = Cube_Raw;
  proc.set_processingArgs(args);

  std::cout << "waiting for camera to become online ...";
  acq.register_state_change_callback(
      [](cuvis::hardware_state_t state,
         std::map<int, cuvis::AcquisitionContext::component_state_info_t>
             comp_states) -> void {
        std::cout << std::endl;
        switch (state)
        {
          case hardware_state_online:
            std::cout << "camera online" << std::endl;
            break;

          case hardware_state_partially_online:
            std::cout << "camera partially online" << std::endl;
            break;

          case hardware_state_offline:
            std::cout << "camera offline" << std::endl;
            break;
        }

        std::cout << " components: " << std::endl;
        for (auto const& comp : comp_states)
        {
          std::cout << " #" << comp.first << " " << comp.second.display_name
                    << " is " << (comp.second.is_online ? "online" : "offline")
                    << std::endl;
        }
      });

  while (acq.get_state() == hardware_state_offline)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::cout << "Waiting for camera to come online ..." << std::endl;
  }
  std::cout << std::endl;

  std::cout << "initializing hardware..." << std::endl;
  acq.set_integration_time(exposure_ms).get();
  acq.set_operation_mode(OperationMode_Internal).get();
  acq.set_fps(fps).get();
  acq.set_auto_exp(autoExp);
  acq.set_continuous(true);

  std::cout << "registering signal for CTRL+c (cancel recording)..."
            << std::endl;
  std::signal(SIGINT, signal_handler);

  std::cout << "configuring worker..." << std::endl;
  cuvis::WorkerArgs worker_settings;
  worker_settings.can_skip_measurements = false; // Worker cannot skip exporting measurements
  worker_settings.can_skip_supplementary_steps = true; // Worker can skip view generation
  worker_settings.can_drop_results = true; // Worker can drop results from the output queue, if it is full
  worker_settings.input_queue_size = 10;
  worker_settings.output_queue_size = 5;
  worker_settings.mandatory_queue_size = 2;
  worker_settings.supplementary_queue_size = 2;

  cuvis::Worker worker(worker_settings);
  worker.set_acq_cont(&acq);

  //comment out the following line to prevent processing
  worker.set_proc_cont(&proc);

  //comment out the following line to prevent saving to disk
  worker.set_exporter(&exporter);

  std::cout << "configuring FPS Analysis Stuff" << std::endl;
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;
  auto t1 = high_resolution_clock::now();
  std::vector<long long> frametimes;
  int fpsAveraging = 200;

  std::cout << "recording...! " << std::endl;
  worker.start_processing();

  while (0 != keepRunning)
  {
    auto workerContainer = worker.get_next_result(100ms);
    // maybe we're already cancelling
    if (keepRunning == 0)
        continue;
    if (workerContainer.mesu.has_value())
    {
      auto t2 = t1;
      t1 = high_resolution_clock::now();

      std::cout
          << "current handle index: "
          << workerContainer.mesu.value().get_meta()->session_info.sequence_no
          << std::endl;

      auto ms_int = duration_cast<milliseconds>(t1 - t2);

      if (frametimes.size() >= fpsAveraging)
      {
        frametimes.erase(frametimes.begin());
      }
      frametimes.push_back(ms_int.count());
      long long totalFrametime = 0;
      for (int i = 0; i < frametimes.size(); i++)
      {
        totalFrametime += frametimes[i];
      }
      double actualFps =
          1 / (((double)totalFrametime / (double)frametimes.size()) / 1000);
      if (abs(actualFps - fps) > 0.5 &&
          frametimes.size() ==
              fpsAveraging) //fps is significantly different from user setting and averaging vector is full
      {
        std::cout << "WARNING: FPS was set to " << fps
                  << " but on average we only get " << actualFps << std::endl;
      }
      if (worker.get_threads_busy() == 4)
      {
        std::cout << "worker queue is full! Main() loop can not keep up!"
                  << std::endl;
      }
      if (acq.get_queue_size() == acq.get_queue_used())
      {
        std::cout << "Acquisition queue is full! Worker can not keep up!"
                  << std::endl;
      }
    }
  }

  signal(SIGINT, SIG_DFL);
  std::cout << "acquisition stopped." << std::endl;
  acq.set_continuous(false);
  acq.reset_state_change_callback();
  cuvis::General::shutdown();
  cuvis::General::reset_log_callback();
  std::cout << std::endl << "finished." << std::endl;
}
