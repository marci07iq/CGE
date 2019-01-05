#include "Framework.h"

Shader baseShader;
Shader edgeShader;

Texture Editor::_editorIcons;
//Plugin types
map<string, PluginCreator> Editor::_pluginTypes;