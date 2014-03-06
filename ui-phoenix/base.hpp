#include <snes.hpp>

#include <nall/base64.hpp>
#include <nall/config.hpp>
#include <nall/directory.hpp>
#include <nall/input.hpp>
#include <nall/ups.hpp>
#include <nall/snes/info.hpp>
using namespace nall;

#include <ruby/ruby.hpp>
using namespace ruby;

#include <phoenix/phoenix.hpp>
using namespace phoenix;

#include "interface.hpp"
#include "config.hpp"
#include "general/general.hpp"
#include "settings/settings.hpp"
#include "tools/tools.hpp"
#include "input/input.hpp"
#include "utility/utility.hpp"
#include "cartridge/cartridge.hpp"

struct Application {
  array<Window*> windows;
  Font proportionalFont;
  Font proportionalFontBold;
  Font monospaceFont;

  bool quit;
  void main(int argc, char **argv);
};

extern Application application;

struct Style {
  enum : unsigned {
  #if defined(PHOENIX_WINDOWS)
    CheckBoxHeight = 15,
    ComboBoxHeight = 22,
    EditBoxHeight = 22,
    LabelHeight = 15,
    SliderHeight = 25,
  #elif defined(PHOENIX_GTK)
    CheckBoxHeight = 15,
    ComboBoxHeight = 22,
    EditBoxHeight = 22,
    LabelHeight = 15,
    SliderHeight = 22,
  #elif defined(PHOENIX_QT)
    CheckBoxHeight = 15,
    ComboBoxHeight = 22,
    EditBoxHeight = 22,
    LabelHeight = 15,
    SliderHeight = 22,
  #endif
  };
};
