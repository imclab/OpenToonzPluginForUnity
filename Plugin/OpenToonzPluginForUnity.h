#ifndef OpenToonzPluginForUnity_h
#define OpenToonzPluginForUnity_h

class otPlugin;

enum otParamType {
    otParamType_Double,
    otParamType_Range,
    otParamType_Pixel,
    otParamType_Point,
    otParamType_Enum,
    otParamType_Int,
    otParamType_Bool,
    otParamType_Spectrum,
    otParamType_String,
    otParamType_ToneCurve,
};
struct otParamInfo
{
    const char *name;
    otParamType type;
};


struct otDoubleParam
{
    double value;
};
struct ouIntParam
{
    int value;
};
struct ouBoolParam
{
    bool value;
};
struct ouStringParam
{
    const char *value;
};
struct otRangeParam
{
    double min, max;
};
struct otColorParam
{
    int value[4];
};
struct ouPointParam
{
    double x, y;
};
struct ouToneCurveParam
{
    double x, y;
    double color[4];
};

union otParamData
{
    otDoubleParam    double_p;
    ouIntParam       int_p;
    ouBoolParam      bool_p;
    ouStringParam    string_p;
    otRangeParam     range_p;
    otColorParam     color_p;
    ouPointParam     point_p;
    ouToneCurveParam tonecurve_p;
};


otPlugin*   otLoadPlugin(const char *path);
void        otUnloadPlugin(otPlugin *plugin);

int         otGetNumParams(otPlugin *plugin);
void        otGetParamInfo(otPlugin *plugin, otParamInfo *pinfo);
void        otApplyEffect(otPlugin *plugin, otParamData *pdata, const void *src_pixels, void *dst_pixels);

#endif // OpenToonzPluginForUnity_h
