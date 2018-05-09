void setup() {
  // put your setup code here, to run once:
   pinMode(13, OUTPUT);
   Serial.begin(115200);
}

uint8_t result;

void loop() {
  // put your main code here, to run repeatedly:
   digitalWrite(13, LOW);
   delay(500);
   digitalWrite(13, HIGH);
   delay(50);
   digitalWrite(13, LOW);
   delay(50);
   digitalWrite(13, HIGH);
   delay(50);
    asm volatile(
      "     clr r20               \n\t" //
      "     lds r16, %[timer0]    \n\t" //
      "     andi r16, 7           \n\t" //
//      "     clr r17 \n\t" //
//      "     lsl r16               \n\t" // not needed because all opcodes are 2-bytes len
      "     ldi r31, pm_hi8(LW)   \n\t" //
      "     ldi r30, pm_lo8(LW)   \n\t" //
      "     add r30, r16          \n\t" // no need to multiply by 2 since all opcodes are 2-bytes
      "     adc r31, __zero_reg__ \n\t" //
      "     ijmp                  \n\t" //
      "LW:                        \n\t" //
      "     inc r20               \n\t" //
      "     inc r20               \n\t" //
      "     inc r20               \n\t" //
      "     inc r20               \n\t" //
      "     inc r20               \n\t" //
      "     inc r20               \n\t" //
      "     inc r20               \n\t" //
//      "     nop                   \n\t" //
//      "     nop                   \n\t" //
//      "     nop                   \n\t" //
//      "     nop                   \n\t" //
//      "     nop                   \n\t" //
//      "     nop                   \n\t" //
//      "     nop                   \n\t" //
//      //"   nop                   \n\t" //
      "LBEND:                     \n\t" //
      " mov %[outvar], r20\n\t"
    : [outvar] "=&r" (result)
    : [timer0] "i" (&TCNT0)
    : "r30", "r31", "r16", "r17", "r20");
    Serial.println((uint8_t)result, HEX);
    Serial.println();
}
