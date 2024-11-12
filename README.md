Whack-a-Mole Game - Arduino Uno

Prosjektbeskrivelse

Dette prosjektet er et DIY “Whack-a-Mole”-spill bygget med en Arduino Uno. Spillet inkluderer LED-er som representerer “moles” som vises tilfeldig, knapper for å “treffe” dem, en buzzer for lydindikasjon, og en 7-segment skjerm for å vise poengsummen.

Komponenter

	•	Arduino Uno
	•	LED-er (en per mole)
	•	Trykk-knapper (en per mole)
	•	Buzzer (for lyd når en mole treffes eller et feiltrykk skjer)
	•	8-sifret 7-segment skjerm (for å vise poengsum)

Oppkobling

Pinnekonfigurasjon

Mole-LED-er og knapper

	•	LED-pins: Kobles til pin 2 og pin 3 på Arduino
	•	Button-pins: Kobles til pin 7 og pin 8 på Arduino

Buzzer

	•	Buzzer-pin: Kobles til pin 13 på Arduino.
	•	Jording: Buzzerens GND går til Arduinoens GND.

7-Segment Skjerm

	•	Data-pin (DIN): Kobles til pin 10 på Arduino
	•	Clock-pin (CLK): Kobles til pin 11 på Arduino
	•	Load-pin (CS): Kobles til pin 12 på Arduino
	•	VCC og GND: Skjermens VCC til Arduino 5V, og GND til Arduino GND.
