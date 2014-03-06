Palette palette;
Interface interface;

const uint8_t Palette::gammaRamp[32] = {
  0x00, 0x01, 0x03, 0x06, 0x0a, 0x0f, 0x15, 0x1c,
  0x24, 0x2d, 0x37, 0x42, 0x4e, 0x5b, 0x69, 0x78,
  0x88, 0x90, 0x98, 0xa0, 0xa8, 0xb0, 0xb8, 0xc0,
  0xc8, 0xd0, 0xd8, 0xe0, 0xe8, 0xf0, 0xf8, 0xff,
};

uint8_t Palette::contrastAdjust(uint8_t input) {
  signed contrast = config.video.contrast - 100;
  signed result = input - contrast + (2 * contrast * input + 127) / 255;
  return max(0, min(255, result));
}

uint8_t Palette::brightnessAdjust(uint8_t input) {
  signed brightness = config.video.brightness - 100;
  signed result = input + brightness;
  return max(0, min(255, result));
}

uint8_t Palette::gammaAdjust(uint8_t input) {
  signed result = (signed)(pow(((double)input / 255.0), (double)config.video.gamma / 100.0) * 255.0 + 0.5);
  return max(0, min(255, result));
}

void Palette::update() {
  for(unsigned i = 0; i < 32768; i++) {
    unsigned r = (i >> 10) & 31;
    unsigned g = (i >>  5) & 31;
    unsigned b = (i >>  0) & 31;

    r = (r << 3) | (r >> 2);
    g = (g << 3) | (g >> 2);
    b = (b << 3) | (b >> 2);

    if(config.video.useGammaRamp) {
      r = gammaRamp[r >> 3];
      g = gammaRamp[g >> 3];
      b = gammaRamp[b >> 3];
    }

    if(config.video.contrast != 100) {
      r = contrastAdjust(r);
      g = contrastAdjust(g);
      b = contrastAdjust(b);
    }

    if(config.video.brightness != 100) {
      r = brightnessAdjust(r);
      g = brightnessAdjust(g);
      b = brightnessAdjust(b);
    }

    if(config.video.gamma != 100) {
      r = gammaAdjust(r);
      g = gammaAdjust(g);
      b = gammaAdjust(b);
    }

    color[i] = (r << 16) | (g << 8) | (b << 0);
  }
}

void Interface::video_refresh(const uint16_t *data, unsigned width, unsigned height) {
  bool interlace = (height >= 240);
  bool overscan = (height == 239 || height == 478);
  unsigned inpitch = interlace ? 1024 : 2048;

  uint32_t *buffer;
  unsigned outpitch;

  if(video.lock(buffer, outpitch, width, height)) {
    for(unsigned y = 0; y < height; y++) {
      uint32_t *output = buffer + y * (outpitch >> 2);
      const uint16_t *input = data + y * (inpitch >> 1);
      for(unsigned x = 0; x < width; x++) *output++ = palette.color[*input++];
    }
    video.unlock();
    video.refresh();
  }

  static signed frameCounter = 0;
  static time_t previous, current;
  frameCounter++;

  time(&current);
  if(current != previous) {
    utility.setStatus(string("FPS: ", frameCounter));
    frameCounter = 0;
    previous = current;
  }
}

void Interface::audio_sample(uint16_t left, uint16_t right) {
  if(config.audio.mute) left = right = 0;
  audio.sample(left, right);
}

void Interface::input_poll() {
}

int16_t Interface::input_poll(bool port, SNES::Input::Device device, unsigned index, unsigned id) {
  if(config.settings.focusPolicy == 1 && mainWindow.focused() == false) return 0;
  return inputMapper.poll(port, device, index, id);
}
