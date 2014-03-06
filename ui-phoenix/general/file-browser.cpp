FileBrowser fileBrowser;

void FileBrowser::create() {
  application.windows.append(this);
  Window::create(0, 0, 256, 256, "Load Cartridge");
  setDefaultFont(application.proportionalFont);

  unsigned x = 5, y = 5;

  unsigned height = Style::EditBoxHeight;
  pathBox.create(*this, x, y, 630 - height - height - 10, height);
  browseButton.create(*this, x + 630 - height - height - 5, y, height, height, "...");
  upButton.create(*this, x + 630 - height, y, height, height, ".."); y += height + 5;

  contentsBox.create(*this, x, y, 630, 350); y += 350 + 5;

  setGeometry(160, 160, 640, y);

  browseButton.onTick = { &FileBrowser::folderBrowse, this };
  upButton.onTick = { &FileBrowser::folderUp, this };
  contentsBox.onActivate = { &FileBrowser::fileActivate, this };
}

void FileBrowser::fileOpen(const char *pathname) {
  setVisible(false);
  setFolder(pathname);
  setVisible(true);
  contentsBox.setFocused();
}

void FileBrowser::setFolder(const char *pathname) {
  contentsBox.reset();
  contents.reset();

  folder = pathname;
  folder.transform("\\", "/");
  folder.rtrim("/");
  pathBox.setText(folder);
  lstring contentsList = directory::contents(folder);
  foreach(item, contentsList) {
    if(strend(item, "/") || strend(item, ".sfc")) contents.append(item);
  }
  foreach(item, contents) contentsBox.addItem(item);
  contentsBox.setSelection(0);
}

void FileBrowser::folderBrowse() {
  string pathname = os.folderSelect(*this, folder);
  if(pathname != "") setFolder(pathname);
}

void FileBrowser::folderUp() {
  setFolder(dir(folder));
}

void FileBrowser::fileActivate() {
  if(auto position = contentsBox.selection()) {
    string filename = contents[position()];
    if(strend(filename, "/")) {
      setFolder(string(folder, "/", filename));
    } else {
      filename = string(folder, "/", filename);
      cartridge.loadNormal(filename);
      SNES::system.power();
      setVisible(false);
    }
  }
}
