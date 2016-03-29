#include "pch.h"
#include "OpenToonzPlugin.h"
#include "fcFoundation.h"
#include "otPlugin.h"

typedef int(*toonz_plugin_init_t)(toonz_host_interface_t *hostif);
typedef void(*toonz_plugin_exit_t)();
extern toonz_host_interface_t g_toonz_host_interface;


otParam::otParam()
{
}

otParam::otParam(const otParamInfo& info)
    : m_info(info)
    , m_data()
{
}

const otParamInfo& otParam::getInfo() const
{
    return m_info;
}

otParamData& otParam::getData()
{
    return m_data;
}




otPlugin::otPlugin(toonz_plugin_probe_t *probe)
    : m_probe(probe)
{
    m_info.name = m_probe->name;
    m_info.vendor = m_probe->vendor;
    m_info.note = m_probe->note;
    m_info.version_major = m_probe->ver.major;
    m_info.version_minor = m_probe->ver.minor;

    if (m_probe->handler && m_probe->handler->setup) {
        m_probe->handler->setup(this);
    }
}

otPlugin::~otPlugin()
{
}

void otPlugin::setParamInfo(toonz_param_page_t *pages, int num_pages)
{
    m_params.clear();
    for (int pi = 0; pi < num_pages; ++pi) {
        toonz_param_page_t& page = pages[pi];
        for (int gi = 0; gi < page.num; ++gi) {
            toonz_param_group_t& group = page.array[gi];
            for (int i = 0; i < group.num; ++i) {
                toonz_param_desc_t& desc = group.array[i];

                otParamInfo info;
                info.name = desc.key;
                info.note = desc.note;
                info.type = (otParamType)desc.traits_tag;
                m_params.push_back(info);
            }
        }
    }
}

const otPluginInfo& otPlugin::getPluginInfo() const
{
    return m_info;
}

int otPlugin::getNumParams() const
{
    return (int)m_params.size();
}

otParam& otPlugin::getParam(int i)
{
    return m_params[i];
}

void otPlugin::applyFx(otParamData *params, otImage *img, double frame)
{
    toonz_rendering_setting_t rs;
    // todo
    m_probe->handler->do_compute(this, &rs, frame, img);
}



otModule::otModule()
    : m_module()
{
}
otModule::~otModule()
{
    unload();
}

bool otModule::load(const char *path)
{
    m_module = DLLLoad(path);
    if (!m_module) { return false; }

    // check if m_module is OpenToonz plug-in
    toonz_plugin_probe_list_t *probes = (toonz_plugin_probe_list_t*)DLLGetSymbol(m_module, "toonz_plugin_info_list");
    if (probes == nullptr) {
        unload();
        return false;
    }

    // call toonz_plugin_init
    auto init_f = (toonz_plugin_init_t)DLLGetSymbol(m_module, "toonz_plugin_init");
    if (init_f) {
        init_f(&g_toonz_host_interface);
    }

    for (auto p = probes->begin; p < probes->end; ++p) {
        m_plugins.emplace_back(otPlugin(p));
    }

    return true;
}

void otModule::unload()
{
    if (m_module) {
        // call toonz_plugin_exit
        auto exit_f = (toonz_plugin_exit_t)DLLGetSymbol(m_module, "toonz_plugin_exit");
        if (exit_f) {
            exit_f();
        }

        DLLUnload(m_module);
        m_module = nullptr;
    }
}

int otModule::getNumPlugins() const
{
    return (int)m_plugins.size();
}

otPlugin* otModule::getPlugin(int i)
{
    if (i >= m_plugins.size()) { return nullptr; }
    return &m_plugins[i];
}