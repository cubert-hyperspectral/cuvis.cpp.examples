#include "cuvis.hpp"

#include <cassert>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[])
{
    {
        /*
        * Expot / Convert Cubert Measurements to different file formats
        * 
        * This example provides information on the exporter classes and the file formats which they can convert measurements to.
        *
        * Used principles:
        *   - *SessionFile* as a source for measurements
        *   - *CubeExporter* for saving measurements
        *   - *TiffExporter* for exporting to TIFF format
        *   - *EnviExporter* for exporting to ENVI format
        *   - *ViewExporter* for exporting rendered views of the data
        *   - *UserPlugins* to define how a view is computed
        *
        * Prerequisites to running this example:
        *   - Have recorded a *SessionFile* (.cu3s) *or* downloaded the provided [demo data](https://cloud.cubert-gmbh.de/s/SDKSampleData)
        *   - Have the Cuvis SDK installed
        *
        * Run properties
        *   - "path/to/settings" "path/to/measurement/file.cu3s" measurementIndex "path/to/user/plugin/file.xml" "OutputFolderName"
        */
        if (argc != 6)
        {
            std::cout << std::endl << "Too few Arguments! Please provide:" << std::endl;
            std::cout << "user settings directory" << std::endl;
            std::cout << "sessionfile (.cu3s)" << std::endl;
            std::cout << "measurement index" << std::endl;
            std::cout << "user plugin file (.xml)" << std::endl;
            std::cout << "Name of export directory" << std::endl;

            return -1;
        }
        char* const userSettingsDir = argv[1];
        char* const sessionLoc = argv[2];
        int const mesuIndex = std::stoi(argv[3]);
        char* const pluginLoc = argv[4];
        char* const exportDir = argv[5];

        std::cout << "Example 04 export measurement" << std::endl;
        std::cout << "User Settings Dir: " << userSettingsDir << std::endl;
        std::cout << "sessionfile (.cu3s): " << sessionLoc << std::endl;
        std::cout << "measurement index: " << mesuIndex << std::endl;
        std::cout << "user plugin file (.xml): " << pluginLoc << std::endl;
        std::cout << "Export Dir: " << exportDir << std::endl;

        std::cout << "loading settings... " << std::endl;
        cuvis::General::init(userSettingsDir);
        cuvis::General::set_log_level(loglevel_info);

        std::cout << "loading session... " << std::endl;
        cuvis::SessionFile sess(sessionLoc);

        std::cout << "loading measurement... " << std::endl;
        auto optmesu = sess.get_mesu(mesuIndex);
        assert(optmesu.has_value());
        cuvis::Measurement mesu = optmesu.value();

        // the exporters need a cube: SessionFiles are saved without it by default (Preview mode), so compute it now
        if (mesu.get_meta()->processing_mode == cuvis::processing_mode_t::Preview)
        {
            cuvis::ProcessingContext proc(sess);
            cuvis::ProcessingArgs procArgs;
            procArgs.processing_mode = cuvis::processing_mode_t::Cube_Raw;
            proc.set_processingArgs(procArgs);
            proc.apply(mesu);
        }

        {
            /*
            * ENVI Exporter
            * For the *EnviExporter* only some basic settings are available.
            * This exporter will create two files per measurement: A `.hdr` file and a data file with the same name but without a file extension.
            * `export_dir` and `channel_selection` apply the same as for *TiffExportSettings* above.
            */
            std::cout << "Export to Envi" << std::endl;
            cuvis::EnviArgs args;
            char exportDirEnvi[CUVIS_MAXBUF];
            strcpy(exportDirEnvi, exportDir);
            strcat(exportDirEnvi, "/envi");
            args.export_dir = exportDirEnvi;
            cuvis::EnviExporter exporter(args);
            exporter.apply(mesu);
        }
        {
            /*
            * TIFF Exporter
            * #### TIFF Exporter
            * The *TiffExporter* is used to export hyperspectral cubes (or parts thereof) in *.tiff* (*.tif*) format. As the TIFF format is fairly flexible, three general modes are available:
            *   - MultiChannel: The cube is stored as a single image in a single file with each wavelength band as a seperate channel
            *   - MultiPage: The cube is stored as multiple single channel (monochrome) images in a single file. Each wavelength band is a separate "page" within the TIFF file.
            *   - Single: The cube is stored as a multiple single channel (monochrome) image in multiple files. Each wavelength band is a separate TIFF file.
            *
            * The TIFF export mode is set via the *TiffExportSettings* class when creating the *TiffExporter*. Here is an overview of further useful settings:
            *   - `export_dir`: The directory to export the measurements to
            *   - `channel_selection`: Select which wavelength bands are included in the export by their wavelength \[nm\] value. This is set via string using a selection syntax. Here are some valid examples:
            *   - Include only the channel at 400nm: "400"
            *   - Include channels at 400, 410 and 620nm: "400;410;620"
            *   - Include all channels from 400 to 500nm (both exclusive): "400-500"
            *   - Include the channels 400 to 500nm and channel at 600nm: "400-500;600"
            *   - Include all channels: "all"
            *   - Include the channels between 400 and 600nm in 20nm steps: "400:20:600"
            *   - `compression_mode`: Which compression scheme to use. To enable compression, set to `TiffCompressionMode.LZW`.
            *   - `format`: Which TIFF export mode to use. Default: `TiffFormat.MultiChannel`
            */
            std::cout << "Export to Multi-Channel Tiff" << std::endl;
            cuvis::TiffArgs args;
            char exportDirMulti[CUVIS_MAXBUF];
            strcpy(exportDirMulti, exportDir);
            strcat(exportDirMulti, "/multi");
            args.export_dir = exportDirMulti;
            args.format = cuvis::tiff_format_t::tiff_format_MultiChannel;
            cuvis::TiffExporter exporter(args);
            exporter.apply(mesu);
        }
        {
            std::cout << "Export to separate Tiffs" << std::endl;
            cuvis::TiffArgs args;
            char exportDirSingle[CUVIS_MAXBUF];
            strcpy(exportDirSingle, exportDir);
            strcat(exportDirSingle, "/single");
            args.export_dir = exportDirSingle;
            args.format = cuvis::tiff_format_t::tiff_format_Single;
            cuvis::TiffExporter exporter(args);
            exporter.apply(mesu);
        }
        {
            /*
            * View Exporter
            * The *ViewExporter* enables rendering and exporting views of the cube data, ie. RGB, Color Infrared, or indices like NDVI.
            * The views are always RGB images.
            * How the view is rendered / computed, is described in using a *UserPlugin* - an XML file with a special syntax that describes data accesses and mathematical operations used to compute the view.
            * These files are described in more detail below.
            *
            * The *ViewExporter* is initialized with the class *ViewExportSettings*, like the other exporters.
            * Here is an overview of some useful settings:
            *    - `export_dir` and `channel_selection`: Same as for *TiffExportSettings* above
            *    - `userplugin`: Either the path to the *UserPlugin* `.xml` file or a valid XML string of a *UserPlugin*
            *    - `pan_failback`: Controls the behavior if no cube is available. If `True`, allows using the panchromatic or preview image to be used as a fallback output. Else, throws an exception instead.
            *
            * User Plugins
            * The *UserPlugin* is a XML schema definition for how a hyperspectral cube is accessed and processed to compute a regular RGB image from it.
            * You can find the XML schema in your Cuvis installation under `Cuvis/user/plugin/userplugin.xsd`
            *
            * A selection of plugins is provided with your installation of Cuvis under `Cuvis/user/plugin`. The plugins differ slightly between processing modes, as some may only be applicable to, e.g. reflectance data.
            * For further information on *UserPlugins*, their syntax and the available operators, please refer to the *Cuvis User Plug-Ins manual* PDF included with your Cuvis installation.
            */
            std::cout << "Export View to file" << std::endl;
            cuvis::ViewArgs args;
            char exportDirView[CUVIS_MAXBUF];
            strcpy(exportDirView, exportDir);
            strcat(exportDirView, "/view");
            args.export_dir = exportDirView;

            std::cout << "Load plugin" << std::endl;
            std::ifstream file(pluginLoc);
            args.userplugin = std::string(
                (std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());

            cuvis::ViewExporter exporter(args);
            exporter.apply(mesu);

            /*
            * View Exporter - Pansharpening
            * Only for cameras with a panchromatic sensor (P models, e.g. Ultris X20P).
            * Pansharpening fuses the cube (high spectral resolution) with the pan image (high spatial resolution).
            * It is set in the pansharpening settings of any exporter. With the *ViewExporter* it is applied after rendering the view, which is fast.
            *    - `pan_scale`: 0.0 = off, 1.0 = resolution of the pan image
            *    - `pan_algorithm`: `CubertMacroPixel` works in all processing modes, `CubertPanRatio` needs Reflectance mode, `Noop` only upscales
            *    - `pre_pan_sharpen_cube`: sharpen the whole cube before rendering (more accurate, much slower)
            */
            if (mesu.get_imdata()->count(CUVIS_MESU_PAN_KEY) > 0) // only with a pan image
            {
                std::cout << "Export pansharpened view to file" << std::endl;
                char exportDirPansharpen[CUVIS_MAXBUF];
                strcpy(exportDirPansharpen, exportDir);
                strcat(exportDirPansharpen, "/pansharpened");
                args.export_dir = exportDirPansharpen;
                args.pansharpening_settings.pan_scale = 1.0;
                args.pansharpening_settings.pan_interpolation_type = pan_sharpening_interpolation_type_Linear;
                args.pansharpening_settings.pan_algorithm = pan_sharpening_algorithm_CubertMacroPixel;
                args.pansharpening_settings.pre_pan_sharpen_cube = false;

                cuvis::ViewExporter pansharpenExporter(args);
                pansharpenExporter.apply(mesu);
            }
        }
        {
            /*
            * Cube Exporter - Settings (SaveArgs)
            * When setting up a *CubeExporter* there are many settings to fine-tune the export process and define what gets saved.
            * The *SaveArgs* class is used to communicate the settings to the *CubeExporter*.
            * 
            * Here is an overview of the most important attributes of *SaveArgs*:
            *    - `export_dir`: The directory to export the measurements to
            *    - `allow_overwrite`: Allow overwriting files in case of a name clash
            *    - `merge_mode`: Controls behavior regarding *SessionFile* creation. Possible values:
            *       - `Default`: Save measurements into a single *SessionFile*
            *       - `Fragmentation`: Start a new *SessionFile* for each measurement / Allow only one measurement per *SessionFile*
            *       - `Merge`: Save measurements into a single *SessionFile*, even if coming from different source files / sessions.
            *    - `allow_drop`: Allow the exporter to drop measurements if it cannot write to the disk fast enough
            *    - `allow_info_file`: Create an info file alongside the *SessionFile*. This file contains a list of all measurements in the *SessionFile* and also marks dropped measurements
            *    - `full_export`: Include the hyperspectral cube when saving to disk. This is **FALSE** by default! *Please note:* The cube can **always be recomputed** on demand from the data stored in the *SessionFile*. Saving the cube can make sense to speed up or allow access to cube data on systems with low or insufficient processing power. It does significantly increase the size of the *SessionFiles*, usually roughly 2x. Additionally, if the cube is included, the processing mode (RAW, Reflectance, ...) is preserved.
            */
            std::cout << "Export to sessionfile" << std::endl;
            char exportDirView[CUVIS_MAXBUF];
            strcpy(exportDirView, exportDir);
            strcat(exportDirView, "/session");

            cuvis::SaveArgs saveArgs;
            saveArgs.export_dir = exportDirView;
            saveArgs.allow_overwrite = true;
            saveArgs.allow_session_file = true;
            saveArgs.full_export = false; // do not include the cube in the export

            cuvis::CubeExporter exporter(saveArgs);
            exporter.apply(mesu);
        }
    }
  cuvis::General::shutdown();
  std::cout << "finished." << std::endl;
}
