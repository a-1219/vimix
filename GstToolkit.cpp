#include <sstream>
#include <iomanip>
using namespace std;

#include <gst/gl/gl.h>

#include "GstToolkit.h"

string GstToolkit::time_to_string(guint64 t, time_string_mode m)
{
    if (t == GST_CLOCK_TIME_NONE) {
        switch (m) {
        case TIME_STRING_FIXED:
            return "00:00:00.00";
        case TIME_STRING_MINIMAL:
            return "0.0";
        default:
            return "00.00";
        }
    }

    guint ms =  GST_TIME_AS_MSECONDS(t);
    guint s = ms / 1000;
    ostringstream oss;

    // MINIMAL: keep only the 2 higher values (most significant)
    if (m == TIME_STRING_MINIMAL) {
        int count = 0;
        if (s / 3600) {
            oss << s / 3600 << ':';
            count++;
        }
        if ((s % 3600) / 60) {
            oss << (s % 3600) / 60 << ':';
            count++;
        }
        if (count < 2) {
            oss << setw(count > 0 ? 2 : 1) << setfill('0') << (s % 3600) % 60;
            count++;
        }
        if (count < 2 )
            oss << '.'<< setw(1) << setfill('0') << (ms % 1000) / 10;
    }
    else {
        // TIME_STRING_FIXED : fixed length string (11 chars) HH:mm:ss.ii"
        // TIME_STRING_RIGHT : always show the right part (seconds), not the min or hours if none
        if (m == TIME_STRING_FIXED || (s / 3600) )
            oss << setw(2) << setfill('0') << s / 3600 << ':';
        if (m == TIME_STRING_FIXED || ((s % 3600) / 60) )
            oss << setw(2) << setfill('0') << (s % 3600) / 60 << ':';
        oss << setw(2) << setfill('0') << (s % 3600) % 60 << '.';
        oss << setw(2) << setfill('0') << (ms % 1000) / 10;
    }

    return oss.str();
}


std::string GstToolkit::filename_to_uri(std::string path)
{
    // set uri to open
    gchar *uritmp = gst_filename_to_uri(path.c_str(), NULL);
    std::string uri( uritmp );
    g_free(uritmp);

    return uri;
}

list<string> GstToolkit::all_plugins()
{
    list<string> pluginlist;
    GList *l, *g;

    l = gst_registry_get_plugin_list (gst_registry_get ());

    for (g = l; g; g = g->next) {
        GstPlugin *plugin = GST_PLUGIN (g->data);
        pluginlist.push_front( string( gst_plugin_get_name (plugin) ) );
    }

    gst_plugin_list_free (l);

    return pluginlist;
}


list<string> GstToolkit::all_plugin_features(string pluginname)
{
    list<string> featurelist;
    GList *l, *g;

    l = gst_registry_get_feature_list_by_plugin (gst_registry_get (), pluginname.c_str());

    for (g = l; g; g = g->next) {
        GstPluginFeature *feature = GST_PLUGIN_FEATURE (g->data);
        featurelist.push_front( string( gst_plugin_feature_get_name (feature) ) );
    }

    gst_plugin_feature_list_free (l);

    return featurelist;
}

bool GstToolkit::enable_feature (string name, bool enable) {
    GstRegistry *registry = NULL;
    GstElementFactory *factory = NULL;

    registry = gst_registry_get();
    if (!registry) return false;

    factory = gst_element_factory_find (name.c_str());
    if (!factory) return false;

    if (enable) {
        gst_plugin_feature_set_rank (GST_PLUGIN_FEATURE (factory), GST_RANK_PRIMARY + 1);
    }
    else {
        gst_plugin_feature_set_rank (GST_PLUGIN_FEATURE (factory), GST_RANK_NONE);
    }

    gst_registry_add_feature (registry, GST_PLUGIN_FEATURE (factory));
    return true;
}


string GstToolkit::gst_version()
{
    std::ostringstream oss;
    oss << GST_VERSION_MAJOR << '.' << GST_VERSION_MINOR << '.';
    oss << std::setw(2) << setfill('0') << GST_VERSION_MICRO ;
    if (GST_VERSION_NANO > 0)
        oss << ( (GST_VERSION_NANO < 2 ) ? " - (CVS)" : " - (Prerelease)");

    return oss.str();
}


// see https://developer.ridgerun.com/wiki/index.php?title=GStreamer_modify_the_elements_rank

std::list<std::string> GstToolkit::enable_gpu_decoding_plugins()
{
    static list<string> pluginslist;
    static GstRegistry* plugins_register = nullptr;

    if ( plugins_register == nullptr ) {
        plugins_register = gst_registry_get();

#if GST_GL_HAVE_PLATFORM_GLX
        // https://gstreamer.freedesktop.org/documentation/nvcodec/index.html?gi-language=c#plugin-nvcodec
        const char *plugins[6] = { "nvh264dec", "nvh265dec", "nvmpeg2videodec",
                                   "nvmpeg4videodec", "nvvp8dec", "nvvp9dec" };
        const int N = 6;
#elif GST_GL_HAVE_PLATFORM_CGL
        const char *plugins[1] = { "vtdec_hw" };
        const int N = 1;
#else
        const char *plugins[0] = { };
        const int N = 0;
#endif

        for (int i = 0; i < N; i++) {
            GstPluginFeature* feature = gst_registry_lookup_feature(plugins_register, plugins[i]);
            if(feature != NULL) {
                pluginslist.push_front( string( plugins[i] ) );
                gst_plugin_feature_set_rank(feature, GST_RANK_PRIMARY + 1);
                gst_object_unref(feature);
            }
        }
    }

    return pluginslist;
}
