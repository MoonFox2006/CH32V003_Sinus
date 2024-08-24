final int SINUS_LEN = 25;
final int PWM_BITS = 7;
final int PWM_MAX = (1 << PWM_BITS) - 1;

int[] sinus = new int[SINUS_LEN];

void generateSinus() {
  float delta = TWO_PI / SINUS_LEN;
  float angle = 0;

  for (int i = 0; i < SINUS_LEN; ++i) {
    sinus[i] = round((sin(angle) + 1.0) * (PWM_MAX / 2));
    angle += delta;
    print(sinus[i]);
    print(", ");
  }
}

void setup() {
  size(128 * 4, 256);
  generateSinus();
}

void draw() {
  for (int i = 1; i < SINUS_LEN; ++i) {
    line((i - 1) * 4, PWM_MAX - sinus[i - 1], i * 4, PWM_MAX - sinus[i]);
  }
}
