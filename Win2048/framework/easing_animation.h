#pragma once
#ifndef EASING_ANIMATION
#define EASING_ANIMATION

#include <math.h>

#define Linear         0
#define EaseInSine     1
#define EaseOutSine    2
#define EaseInOutSine  3
#define EaseInQuad     4
#define EaseOutQuad    5
#define EaseInOutQuad  6
#define EaseInCubic    7
#define EaseOutCubic   8
#define EaseInOutCubic 9
#define EaseInQuart    10
#define EaseOutQuart   11
#define EaseInOutQuart 12
#define EaseInQuint    13
#define EaseOutQuint   14
#define EaseInOutQuint 15
#define EaseInExpo     16
#define EaseOutExpo    17
#define EaseInOutExpo  18
#define EaseInCirc     19
#define EaseOutCirc    20
#define EaseInOutCirc  21

#define PI 3.14159265358979

#define ANIMATIONS_MAX 5

class EasingAnimation {
public:
  double Value = 0.0;

  void startEasing(double _StartValue, double _EndValue, int _EasingFunction, double _Duration, double _Delay)
  {
    for (int i = 0; i < ANIMATIONS_MAX; i++) {
      if (!Animations[i].Activated) {
        Animations[i].startEasing(&Value, _StartValue, _EndValue, _EasingFunction, _Duration, _Delay);
        return;
      }
    }
  }

  void startEasing(double _EndValue, int _EasingFunction, double _Duration, double _Delay)
  {
    for (int i = 0; i < ANIMATIONS_MAX; i++) {
      if (!Animations[i].Activated) {
        Animations[i].startEasing(&Value, _EndValue, _EasingFunction, _Duration, _Delay);
        return;
      }
    }
  }

  void abortEasing() {
    for (int i = 0; i < ANIMATIONS_MAX; i++) {
      if (Animations[i].Activated) {
        Animations[i].abortEasing();
      }
    }
  }

  double updateWithTotalTime(double _Time) {
    for (int i = 0; i < ANIMATIONS_MAX; i++) {
      if (Animations[i].Activated) {
        Animations[i].updateWithTotalTime(_Time);
      }
    }
    return Value;
  }

  double updateWithInterval(double _Interval) {
    for (int i = 0; i < ANIMATIONS_MAX; i++) {
      if (Animations[i].Activated) {
        Animations[i].updateWithInterval(_Interval);
      }
    }
    return Value;
  }

  bool isActive() {
    for (int i = 0; i < ANIMATIONS_MAX; i++) {
      if (Animations[i].Activated) {
        return true;
      }
    }
    return false;
  }

private:
  class Animation {
  public:
    bool Activated = false;

    void startEasing(double* _Value, double _StartValue, double _EndValue, int _EasingFunction, double _Duration, double _Delay)
    {
      Value = _Value;
      StartValueSet = true;
      StartValue = _StartValue;
      EndValue = _EndValue;
      Duration = _Duration;
      Delay = _Delay;
      EasingFunction = _EasingFunction;
      CurrentTime = 0.0;
      Activated = true;
    }
    
    void startEasing(double* _Value, double _EndValue, int _EasingFunction, double _Duration, double _Delay)
    {
      Value = _Value;
      StartValueSet = false;
      EndValue = _EndValue;
      Duration = _Duration;
      Delay = _Delay;
      EasingFunction = _EasingFunction;
      CurrentTime = 0.0;
      Activated = true;
    }

    void abortEasing() {
      Activated = false;
    }

    void updateWithTotalTime(double _Time) {
      if (!Activated)
        return;
      CurrentTime = _Time;
      double animationTime = CurrentTime - Delay;
      if (animationTime <= 0.0)
        return;
      if (animationTime >= Duration) {
        Activated = false;
        CurrentTime = Duration + Delay;
        *Value = EndValue;
        return;
      }
      if (!StartValueSet) {
        StartValueSet = true;
        StartValue = *Value;
      }
      *Value = StartValue + (EndValue - StartValue) * valueFromFunction(animationTime / Duration);
    }

    void updateWithInterval(double _Interval) {
      if (!Activated)
        return;
      updateWithTotalTime(CurrentTime + _Interval);
    }

  private:
    double* Value = nullptr;

    bool StartValueSet = false;
    double StartValue = 0.0;
    double EndValue = 0.0;
    double Duration = 0.0;
    double Delay = 0.0;
    double CurrentTime = 0.0;
    int EasingFunction = Linear;

    double valueFromFunction(double _X) {
      if (_X < 0.0f) _X = 0.0f;
      else if (_X > 1.0f) _X = 1.0f;
      double dat = _X - 1.0f;
      switch (EasingFunction) {
      case Linear:
        return _X;
      case EaseInSine:
        return 1.0f + sin((_X - 1.0f) * (PI / 2.0f));
      case EaseOutSine:
        return sin(_X * (PI / 2.0f));
      case EaseInOutSine:
        return (1.0f + sin((_X - 0.5f) * PI)) / 2.0f;
      case EaseInQuad:
        return _X * _X;
      case EaseOutQuad:
        return 1.0f - dat * dat;
      case EaseInOutQuad:
        return _X < 0.5f ? (2.0f * _X * _X) : (1.0f - 2.0f * dat * dat);
      case EaseInCubic:
        return _X * _X * _X;
      case EaseOutCubic:
        return 1.0f + dat * dat * dat;
      case EaseInOutCubic:
        return _X < 0.5f ? (4.0f * _X * _X * _X) : (1.0f + 4.0f * dat * dat * dat);
      case EaseInQuart:
        return _X * _X * _X * _X;
      case EaseOutQuart:
        return 1.0f - dat * dat * dat * dat;
      case EaseInOutQuart:
        return _X < 0.5f ? (8.0f * _X * _X * _X * _X) : (1.0f - 8.0f * dat * dat * dat * dat);
      case EaseInQuint:
        return _X * _X * _X * _X * _X;
      case EaseOutQuint:
        return 1.0f + dat * dat * dat * dat * dat;
      case EaseInOutQuint:
        return _X < 0.5f ? (16.0f * _X * _X * _X * _X * _X) : (1.0f + 16.0f * dat * dat * dat * dat * dat);
      case EaseInCirc:
        return 1.0f - sqrt(1.0f - _X * _X);
      case EaseOutCirc:
        return sqrt(1.0f - dat * dat);
      case EaseInOutCirc:
        return _X < 0.5 ? (0.5f - sqrt(0.25f - _X * _X)) : (0.5f + sqrt(0.25f - dat * dat));
      default:
        return 0.0;
      }
    }
  };

  Animation Animations[ANIMATIONS_MAX];
};

#endif  //  EASING_ANIMATION