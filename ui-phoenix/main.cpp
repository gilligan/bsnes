#include "base.hpp"
#include "interface.cpp"
#include "config.cpp"
Application application;

void Application::main(int argc, char **argv) {
  #if defined(PLATFORM_WIN)
  utf8_args(argc, argv);
  #endif
  config.create();
  inputMapper.create();

  char temp[PATH_MAX];
  config.path.base = realpath(argv[0], temp);
  config.path.base.transform("\\", "/");
  config.path.base = dir(config.path.base);
  config.path.user = userpath(temp);
  config.path.user.transform("\\", "/");
  if(strend(config.path.user, "/") == false) config.path.user.append("/");
  config.path.user.append(".bsnes/");
  config.load();
  config.save();
  if(config.path.current == "") config.path.current = config.path.base;
  inputMapper.bind();

  #if defined(PHOENIX_WINDOWS)
  proportionalFont.create("Tahoma", 8);
  proportionalFontBold.create("Tahoma", 8, Font::Style::Bold);
  monospaceFont.create("Courier New", 8);
  #else
  proportionalFont.create("Sans", 8);
  proportionalFontBold.create("Tahoma", 8, Font::Style::Bold);
  monospaceFont.create("Liberation Mono", 8);
  #endif

  if(config.video.driver == "") config.video.driver = video.default_driver();
  if(config.audio.driver == "") config.audio.driver = audio.default_driver();
  if(config.input.driver == "") config.input.driver = video.default_driver();

  palette.update();
  mainWindow.create();
  fileBrowser.create();
  videoSettings.create();
  inputSettings.create();
  advancedSettings.create();
  cheatEditor.create();
  mainWindow.setVisible();
  os.run();

  video.driver(config.video.driver);
  video.set(Video::Handle, mainWindow.viewport.handle());
  video.set(Video::Synchronize, config.video.synchronize);
  video.set(Video::Filter, (unsigned)Video::FilterLinear);
  if(video.init() == false) {
    MessageWindow::critical(mainWindow, "Failed to initialize video.");
    video.driver("None");
    video.init();
  }

  audio.driver(config.audio.driver);
  audio.set(Audio::Handle, mainWindow.viewport.handle());
  audio.set(Audio::Synchronize, config.audio.synchronize);
  audio.set(Audio::Frequency, (unsigned)32000);
  if(audio.init() == false) {
    MessageWindow::critical(mainWindow, "Failed to initialize audio.");
    audio.driver("None");
    audio.init();
  }

  input.driver(config.input.driver);
  input.set(Input::Handle, mainWindow.viewport.handle());
  if(input.init() == false) {
    MessageWindow::critical(mainWindow, "Failed to initialize input.");
    input.driver("None");
    input.init();
  }

  SNES::system.init(&interface);
  if(argc == 2) {
    cartridge.loadNormal(argv[1]);
    SNES::system.power();
  }

  while(quit == false) {
    os.run();
    inputMapper.poll();
    utility.updateStatus();

    if(SNES::cartridge.loaded()) {
      //pause emulator when main window is inactive?
      if(config.settings.focusPolicy == 0) {
        if(mainWindow.focused() == false) {
          usleep(20 * 1000);
          continue;
        }
      }
      SNES::system.run();
    } else {
      usleep(20 * 1000);
    }
  }

  cartridge.unload();
  foreach(window, windows) window->setVisible(false);
  os.run();
  SNES::system.term();
  config.save();

  video.term();
  audio.term();
  input.term();
}

int main(int argc, char **argv) {
  application.main(argc, argv);
  return 0;
}
