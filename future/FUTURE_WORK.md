# Future Work

Dieses Dokument sammelt Erweiterungen, die **nach der Entwicklung des Grundkonzepts** in das Projekt einfliessen sollen.
Es beschreibt bewusst keinen aktuellen v1-Sofortumfang, sondern einen geordneten Ausblick fuer spaetere Ausbaustufen.

## Zielbild

Nach dem Grundsystem soll aus dem heutigen IMU-gesteuerten Roboterarm schrittweise ein groesseres, mobileres und allgemeiner uebertragbares Robotiksystem werden.

Die wichtigsten Zukunftslinien sind:

- fahrbarer oder bewegbarer Untergrund fuer das Gesamtsystem
- Dashboard mit Controller-Anbindung
- Kamerasicht fuer Fahrzeug und Roboterarm
- abstrahierte Aktuator-Schicht statt direkter starrer `IMU -> Servo`-Kopplung

## Zukunftslinie 1 - Fahrbarer oder bewegbarer Untergrund

### Ziel

Der Roboterarm soll spaeter nicht nur stationaer arbeiten, sondern auf einem verfahrbaren Unterbau sitzen oder mit einem beweglichen Fahrgestell kombiniert werden.

### Gewuenschte Eigenschaften

- eigenstaendig programmierbarer Fahruntergrund
- klare Trennung zwischen Fahrplattform und Roboterarm-Subsystem
- spaetere Erweiterbarkeit fuer unterschiedliche Fahrkonzepte
- mechanisch und elektrisch sauber an das bestehende System ankoppelbar

### Typische spaetere Arbeitspakete

- Auswahl eines geeigneten Unterbaus oder Chassis
- Stromversorgungsstrategie fuer Arm und Fahrplattform
- Kommunikationspfad zwischen Armsteuerung und Fahrplattform
- Sicherheitsregeln fuer gleichzeitige Arm- und Fahrbewegung

## Zukunftslinie 2 - Dashboard und Controller-Ebene

### Ziel

Das Gesamtsystem soll spaeter ueber ein Dashboard und physische Controller bedienbar werden.
Dabei soll nicht nur der Roboterarm, sondern auch die mobile Plattform steuerbar sein.

### Gewuenschte Eigenschaften

- zentraler Bedienpunkt fuer Status, Modi und Live-Zustand
- Controller-Eingaben fuer Fahrzeug und Arm
- umschaltbare oder kombinierbare Bedienmodi
- spaetere Teleoperations- oder Assistenzmodi moeglich

### Typische spaetere Arbeitspakete

- Dashboard-Architektur definieren
- Bedienlogik fuer Arm- und Fahrzeugmodus entwerfen
- Statusdarstellung fuer Sensorik, Funk, Safety und Betriebsmodus planen
- Controller-Mapping fuer unterschiedliche Bediengeraete beschreiben

## Zukunftslinie 3 - Kamerasicht und Teleoperationssicht

### Ziel

Das System soll spaeter eine oder mehrere Kameraperspektiven bereitstellen, damit Fahrzeug und Roboterarm nicht nur lokal, sondern mit Sichtunterstuetzung gesteuert werden koennen.

### Gewuenschte Eigenschaften

- Kamerasicht auf Fahrplattform und Arbeitsbereich des Arms
- spaetere Nutzung fuer Dashboard oder Controller-gestuetzte Bedienung
- vorbereiteter Video- oder Sichtpfad fuer Fern- oder Nahsteuerung
- sauber getrennte Zustandsanzeige fuer Kamera, Steuerung und Arm

### Typische spaetere Arbeitspakete

- Kamerapositionen und Sichtzonen festlegen
- Transportpfad fuer Videobild beschreiben
- Dashboard-Integration der Sichtdaten planen
- Bedienlogik zwischen Kamerasicht, Fahrzeugsteuerung und Armsteuerung abstimmen

## Zukunftslinie 4 - Uebertragbare Bewegungs- und Aktuatorabstraktion

### Problem des direkten Pfads

Im heutigen Grundsystem liegt die fachliche Gefahr darin, eine zu direkte Kette `IMU -> Servo-Zielwert` zu bauen.
Das waere fuer den ersten Prototyp zwar schnell, koppelt die Software aber zu stark an genau eine Servo- und Roboterkonfiguration.

### Ziel

Zwischen Bewegungsinterpretation und konkreter Servoausgabe soll spaeter ein eigener Abstraktionslayer liegen, damit das System leichter auf andere Servos, Aktuatoren oder Roboterplattformen uebertragbar wird.

### Gewuenschte Schichten

#### 1. Sensor- und Bewegungsinterpretation

- IMUs, Flex-Sensor und spaetere Controller-Eingaben werden in gemeinsame Bewegungsabsichten uebersetzt

#### 2. Intent- oder Motion-Layer

- aus Rohdaten werden fachliche Zielgroessen wie Gelenkintention, Greifintention oder Plattformbewegung

#### 3. Aktuator-Abstraktionslayer

- fachliche Zielgroessen werden in ein allgemeines Ausgabemodell fuer verschiedene Aktuatortypen uebersetzt

#### 4. Treiber- oder Adapterlayer

- konkrete Servo-, Motor- oder Robotiktreiber setzen die abstrakten Zielgroessen erst ganz am Ende in hardwarebezogene Signale um

### Nutzen

- bessere Uebertragbarkeit auf andere Servos
- bessere Uebertragbarkeit auf andere Roboterarme
- spaetere Unterstuetzung kombinierter Systeme aus Fahrplattform und Arm
- saubere Trennung zwischen Fachlogik und Hardwarebindung

## Zusammenspiel der Zukunftslinien

Das langfristige Ziel ist kein einzelner isolierter Roboterarm mehr, sondern ein steuerbares Gesamtsystem:

```text
Controller / Dashboard / Kamera
              |
       Bewegungs- und Intent-Layer
              |
   +----------+-----------+
   |                      |
Arm-Aktuatoradapter   Fahrzeug-Aktuatoradapter
   |                      |
Roboterarm            Fahrplattform
```

## Regeln fuer spaetere Aktivierung

- kein Zukunftspunkt wird stillschweigend Teil von v1
- sobald ein Punkt aktiv wird, muessen Architektur, Sicherheit, Kommunikation, Hardware und Tests nachgezogen werden
- die Aktuatorabstraktion soll spaeter als eigener Architekturpfad dokumentiert werden, bevor sie in Firmware gegossen wird
- Kamera- und Dashboard-Funktionen duerfen nicht unstrukturiert in bestehende v1-Pfade hineingemischt werden

## Grundlage dieses Dokuments

Dieses Dokument beschreibt bewusst einen internen Zukunftsausblick.
Sobald einzelne Punkte aktiv in Planung oder Recherche gehen, muessen dafuer technische Quellen, Sicherheitsannahmen und konkrete Teil-Roadmaps nachgezogen werden.
