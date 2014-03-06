#include "../base.hpp"
Utility utility;

void Utility::setTitle(const char *text) {
  if(*text) {
    mainWindow.setTitle(string(text, " - ", SNES::Info::Name, " v", SNES::Info::Version));
  } else {
    mainWindow.setTitle(string(SNES::Info::Name, " v", SNES::Info::Version));
  }
}

void Utility::updateStatus() {
  time_t currentTime = time(0);
  string text = ((currentTime - statusTime) > 3) ? statusText : statusMessage;
  if(text != statusCurrentText) {
    mainWindow.setStatusText(statusCurrentText = text);
  }
}

void Utility::setStatus(const char *text) {
  static char profile[] = { '[', SNES::Info::Profile[0], ']', ' ', 0 };
  statusText = string(profile, text);
}

void Utility::showMessage(const char *text) {
  statusMessage = text;
  statusTime = time(0);
}

void Utility::loadCartridgeNormal() {
  if(config.settings.useNativeDialogs == false) {
    fileBrowser.fileOpen(config.path.current);
  } else {
    string filename = os.fileOpen(mainWindow, "SNES cartridges\t*.sfc\nAll files\t*", config.path.current);
    if(filename != "") {
      cartridge.loadNormal(filename);
      SNES::system.power();
    }
  }
}

void Utility::saveState(unsigned slot) {
  string filename = { cartridge.baseName, "-", slot, ".bst" };
  SNES::system.runtosave();
  serializer s = SNES::system.serialize();
  file fp;
  if(fp.open(filename, file::mode_write)) {
    fp.write(s.data(), s.size());
    fp.close();
    showMessage(string("Saved state ", slot));
  } else {
    showMessage(string("Failed to save state ", slot));
  }
}

void Utility::loadState(unsigned slot) {
  string filename = { cartridge.baseName, "-", slot, ".bst" };
  file fp;
  if(fp.open(filename, file::mode_read)) {
    unsigned size = fp.size();
    uint8_t *data = new uint8_t[size];
    fp.read(data, size);
    fp.close();
    serializer s(data, size);
    delete[] data;
    if(SNES::system.unserialize(s) == true) {
      showMessage(string("Loaded state ", slot));
    } else {
      showMessage(string("Failed to load state ", slot));
    }
  }
}

Utility::Utility() {
  statusTime = 0;
}
