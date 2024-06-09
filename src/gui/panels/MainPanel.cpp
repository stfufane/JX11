#include "MainPanel.h"

namespace JX11::Gui
{

MainPanel::MainPanel(Processor::JX11AudioProcessor& p, nlohmann::json& guiData)
    : IPanel("Main Panel", guiData),
      oscPanel { p, guiData },
      filterPanel { p, guiData }
{
    addChild(&oscPanel);
    addChild(&filterPanel);
    addChild(&tooltipPanel);
}

} // namespace JX11::Gui