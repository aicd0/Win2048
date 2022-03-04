#pragma once
#include "framework.h"
#include "framework/game.h"
#include <comdef.h>
#include <gdiplus.h>
#include <atlimage.h>
#include <sstream>
#include <string>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

struct PaintContext {
  Win2048* game;
  bool keyboard_mode;
};

class PaintCore {
public:
  PaintCore() {
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    WhiteBrush = new SolidBrush(Color(255, 255, 255, 255));
    DarkGrayBrush = new SolidBrush(Color(255, 150, 150, 150));
    LightGrayBrush = new SolidBrush(Color(255, 200, 200, 200));
    ThemeBrush = new SolidBrush(Color(255, 224, 186, 1));
    Block2Brush = new SolidBrush(Color(255, 238, 228, 218));
    Block2TextBrush = new SolidBrush(Color(255, 117, 107, 97));
    Block4Brush = new SolidBrush(Color(255, 238, 223, 202));
    Block4TextBrush = new SolidBrush(Color(255, 124, 116, 105));
    Block8Brush = new SolidBrush(Color(255, 242, 177, 121));
    Block16Brush = new SolidBrush(Color(255, 236, 141, 85));
    Block32Brush = new SolidBrush(Color(255, 245, 124, 95));
    Block64Brush = new SolidBrush(Color(255, 234, 89, 58));
    Block128Brush = new SolidBrush(Color(255, 237, 206, 113));
    Block256Brush = new SolidBrush(Color(255, 237, 204, 97));
    Block512Brush = new SolidBrush(Color(255, 236, 200, 80));
    Block1024Brush = new SolidBrush(Color(255, 237, 197, 63));
    Block2048Brush = new SolidBrush(Color(255, 238, 194, 46));
    Block4096Brush = new SolidBrush(Color(255, 255, 61, 61));
    Block8192Brush = new SolidBrush(Color(255, 255, 30, 28));

    hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255));

    hNullPen = (HPEN)GetStockObject(NULL_PEN);
    hBlackPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
  }

  ~PaintCore() {
    delete WhiteBrush;
    delete DarkGrayBrush;
    delete LightGrayBrush;
    delete ThemeBrush;
    delete Block2Brush;
    delete Block2TextBrush;
    delete Block4Brush;
    delete Block4TextBrush;
    delete Block8Brush;
    delete Block16Brush;
    delete Block32Brush;
    delete Block64Brush;
    delete Block128Brush;
    delete Block256Brush;
    delete Block512Brush;
    delete Block1024Brush;
    delete Block2048Brush;
    delete Block4096Brush;
    delete Block8192Brush;

    GdiplusShutdown(m_gdiplusToken);
  }

  bool paint(HDC hdc, PaintContext ctx, int screenX, int screenY, double interval)
  {
    m_currentLine = 0;

    double shortSide = screenY > screenX / 1.5 ? screenX / 1.5 : screenY;
    double tableInset = shortSide * TableInsetConst;
    double blockInset = shortSide * BlockInsetConst;
    double tableRoundRadius = shortSide * TableRoundRadiusConst;
    double blockRoundRadius = shortSide * BlockRoundRadiusConst;
    double scoreBoardWidth = shortSide * ScoreBoardWidthConst;
    double scoreBoardHeight = shortSide * ScoreBoardHeightConst;

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

    SelectObject(hdc, hNullPen);
    SelectObject(hdc, hWhiteBrush);
    Rectangle(hdc, 0, 0, screenX, screenY);

    double length = shortSide - 2 * tableInset;
    double tableStartX = (screenX - length + tableInset + scoreBoardWidth) / 2.0;
    double tableStartY = (screenY - length) / 2.0;
    FillRoundRect(graphics, DarkGrayBrush,
      Rect((int)tableStartX, (int)tableStartY, (int)length, (int)length), (int)tableRoundRadius);
    
    double blockLength = (length - blockInset) / GAME_TABLE_WIDTH - blockInset;
    for (int x = 0; x < GAME_TABLE_WIDTH; x++) {
      for (int y = 0; y < GAME_TABLE_HEIGHT; y++) {
        Rect rect(
          int(tableStartX + x * (blockLength + blockInset) + blockInset),
          int(tableStartY + y * (blockLength + blockInset) + blockInset),
          (int)blockLength, (int)blockLength);
        FillRoundRect(graphics, LightGrayBrush, rect, (int)blockRoundRadius);
      }
    }

    for (int x = 0; x < GAME_TABLE_WIDTH; x++) {
      for (int y = 0; y < GAME_TABLE_HEIGHT; y++) {
        if (ctx.game->GameTable[x][y]) {
          Block& block = *ctx.game->GameTable[x][y];

          double currentBlockLength = blockLength * block.Zoom.updateWithInterval(interval);
          double posX = tableStartX + x * (blockLength + blockInset) + blockInset;
          double posY = tableStartY + y * (blockLength + blockInset) + blockInset;

          if (block.BlockMoved) {
            block.BlockMoved = false;
            block.PositionX.abortEasing();
            block.PositionX.startEasing(posX, EaseOutCubic, 0.4, 0.0);
            block.PositionY.abortEasing();
            block.PositionY.startEasing(posY, EaseOutCubic, 0.4, 0.0);
          }
          if (!block.PositionX.isActive()) {
            block.PositionX.Value = posX;
            block.PositionY.Value = posY;
          }

          Rect rect(
            int(block.PositionX.updateWithInterval(interval) + (blockLength - currentBlockLength) / 2.0),
            int(block.PositionY.updateWithInterval(interval) + (blockLength - currentBlockLength) / 2.0),
            (int)currentBlockLength, (int)currentBlockLength);

          PaintBlock(graphics, rect, ctx.game->GameTable[x][y]->Num, shortSide);
        }
      }
    }

    //  score board
    double scoreBoardStartX = tableStartX - tableInset - scoreBoardWidth;
    double scoreBoardStartY = tableStartY;
    if (ctx.game->GameOver) {
      FillRoundRect(graphics, Block8192Brush, Rect((int)scoreBoardStartX, (int)scoreBoardStartY,
        (int)scoreBoardWidth, (int)scoreBoardHeight), (int)tableRoundRadius);
    }
    else {
      FillRoundRect(graphics, ThemeBrush, Rect((int)scoreBoardStartX, (int)scoreBoardStartY,
        (int)scoreBoardWidth, (int)scoreBoardHeight), (int)tableRoundRadius);
    }

    wchar_t scoreText[20];
    int textLen = swprintf_s<20>(scoreText, L"%d", ctx.game->Score);
    REAL fontSize = (float)shortSide * 0.04f;
    Gdiplus::Font scoreFont(L"SF Pro Display", fontSize, FontStyleBold);
    PointF textOrigin;
    RectF textBound;
    graphics.MeasureString(scoreText, textLen, &scoreFont, textOrigin, &textBound);
    REAL TextOffetY = 5.0f * (float)shortSide / 750.0f;
    graphics.DrawString(scoreText, textLen, &scoreFont, PointF(
      REAL(scoreBoardStartX + (scoreBoardWidth - textBound.Width) / 2.0),
      REAL(scoreBoardStartY + (scoreBoardHeight - textBound.Height) / 2.0) + TextOffetY), WhiteBrush);
    if (ctx.game->GameOver) {
      Gdiplus::Font textFont(L"SF Pro Display", (float)shortSide * 0.015f, FontStyleBold);
      graphics.MeasureString(L"Press ENTER to restart", 22, &textFont, textOrigin, &textBound);
      TextOffetY = 40.0f * (float)shortSide / 750.0f;
      graphics.DrawString(L"Press ENTER to restart", 22, &textFont, PointF(
        REAL(scoreBoardStartX + (scoreBoardWidth - textBound.Width) / 2.0),
        REAL(scoreBoardStartY + (scoreBoardHeight - textBound.Height) / 2.0) + TextOffetY), WhiteBrush);
    }

    // Update FPS
    m_fpsCount++;
    m_timeAccumulated += interval;
    if (m_timeAccumulated >= 1.0) {
      m_fps = m_fpsCount;
      m_fpsCount = 0;
      m_timeAccumulated -= 1.0;
    }

    // Draw text
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    std::stringstream text_stream;

    text_stream << m_fps << " FPS\n";
    text_stream << (ctx.keyboard_mode ? "Keyboard" : "Auto") << " mode (Space)" << "\n";
    text_stream << "(ENTER to reset the game)" << "\n";

    std::string text = text_stream.str();
    text_out(hdc, text);

    return true;
  }

private:
  ULONG_PTR m_gdiplusToken;

  int m_fps = 0;
  int m_fpsCount = 0;
  double m_timeAccumulated = 0.0;
  int m_currentLine = 0;

  const double TableInsetConst = 0.07692;
  const double BlockInsetConst = 0.02769;
  const double TableRoundRadiusConst = 0.02308;
  const double BlockRoundRadiusConst = 0.00923;
  const double ScoreBoardWidthConst = 0.4;
  const double ScoreBoardHeightConst = 0.15;

  SolidBrush* WhiteBrush;
  SolidBrush* DarkGrayBrush;
  SolidBrush* LightGrayBrush;
  SolidBrush* ThemeBrush;
  SolidBrush* Block2Brush;
  SolidBrush* Block2TextBrush;
  SolidBrush* Block4Brush;
  SolidBrush* Block4TextBrush;
  SolidBrush* Block8Brush;
  SolidBrush* Block16Brush;
  SolidBrush* Block32Brush;
  SolidBrush* Block64Brush;
  SolidBrush* Block128Brush;
  SolidBrush* Block256Brush;
  SolidBrush* Block512Brush;
  SolidBrush* Block1024Brush;
  SolidBrush* Block2048Brush;
  SolidBrush* Block4096Brush;
  SolidBrush* Block8192Brush;

  HBRUSH hWhiteBrush;

  HPEN hNullPen;
  HPEN hBlackPen;

  void text_out(HDC hdc, std::string _text) {
    size_t offset = 0;
    for (;;) {
      size_t end = _text.find('\n', offset);
      if (end == std::string::npos) {
        end = _text.size();
      }

      std::string substr = _text.substr(offset, end - offset);
      TextOutA(hdc, 0, m_currentLine * 20, substr.c_str(), (int)substr.size());
      m_currentLine++;

      if (end == _text.size()) {
        break;
      }
      offset = end + 1;
    }
  }

  void FillRoundRect(Graphics& graphics, SolidBrush* brush, Rect rect, int radius) {
    int diam = 2 * radius;
    graphics.FillRectangle(brush, rect.X + radius, rect.Y, rect.Width - diam, rect.Height);
    graphics.FillRectangle(brush, rect.X, rect.Y + radius, rect.Width, rect.Height - diam);
    graphics.FillEllipse(brush, rect.X, rect.Y, diam, diam);
    graphics.FillEllipse(brush, rect.X + rect.Width - diam, rect.Y, diam, diam);
    graphics.FillEllipse(brush, rect.X, rect.Y + rect.Height - diam, diam, diam);
    graphics.FillEllipse(brush, rect.X + rect.Width - diam, rect.Y + rect.Height - diam, diam, diam);
  }

  void PaintBlock(Graphics& graphics, Rect rect, int num, double shortSide)
  {
    int blockRoundRadius = int(rect.Width * BlockRoundRadiusConst * 5.76923);
    switch (num) {
    case 2:
      FillRoundRect(graphics, Block2Brush, rect, blockRoundRadius);
      break;
    case 4:
      FillRoundRect(graphics, Block4Brush, rect, blockRoundRadius);
      break;
    case 8:
      FillRoundRect(graphics, Block8Brush, rect, blockRoundRadius);
      break;
    case 16:
      FillRoundRect(graphics, Block16Brush, rect, blockRoundRadius);
      break;
    case 32:
      FillRoundRect(graphics, Block32Brush, rect, blockRoundRadius);
      break;
    case 64:
      FillRoundRect(graphics, Block64Brush, rect, blockRoundRadius);
      break;
    case 128:
      FillRoundRect(graphics, Block128Brush, rect, blockRoundRadius);
      break;
    case 256:
      FillRoundRect(graphics, Block256Brush, rect, blockRoundRadius);
      break;
    case 512:
      FillRoundRect(graphics, Block512Brush, rect, blockRoundRadius);
      break;
    case 1024:
      FillRoundRect(graphics, Block1024Brush, rect, blockRoundRadius);
      break;
    case 2048:
      FillRoundRect(graphics, Block2048Brush, rect, blockRoundRadius);
      break;
    case 4096:
      FillRoundRect(graphics, Block4096Brush, rect, blockRoundRadius);
      break;
    default:
      FillRoundRect(graphics, Block8192Brush, rect, blockRoundRadius);
    }

    wchar_t numText[10];
    int textLen = swprintf_s<10>(numText, L"%d", num);
    REAL fontSize = (36.0f / textLen + 16.0f) * rect.Width / 130.0f;
    Gdiplus::Font numFont(L"SF Pro Display", fontSize, FontStyleBold);
    PointF textOrigin;
    RectF textBound;
    graphics.MeasureString(numText, textLen, &numFont, textOrigin, &textBound);
    const REAL TextOffetY = 5.0f * (float)shortSide / 750.0f;

    switch (num) {
    case 2:
      graphics.DrawString(numText, textLen, &numFont, PointF(
        (REAL)rect.X + (rect.Width - textBound.Width) / 2,
        (REAL)rect.Y + (rect.Height - textBound.Height) / 2 + TextOffetY), Block2TextBrush);
      break;
    case 4:
      graphics.DrawString(numText, textLen, &numFont, PointF(
        (REAL)rect.X + (rect.Width - textBound.Width) / 2,
        (REAL)rect.Y + (rect.Height - textBound.Height) / 2 + TextOffetY), Block4TextBrush);
      break;
    default:
      graphics.DrawString(numText, textLen, &numFont, PointF(
        (REAL)rect.X + (rect.Width - textBound.Width) / 2,
        (REAL)rect.Y + (rect.Height - textBound.Height) / 2 + TextOffetY), WhiteBrush);
    }
  }
};