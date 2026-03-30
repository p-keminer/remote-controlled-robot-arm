// ============================================================
// js/robotarm_3d.js — Adeept 5-DOF Roboterarm 3D-Visualisierung
// ============================================================
// Realistisches Modell des Adeept 5-DOF Arms (ADA031 Kit V4.0)
// in einem Wohnzimmer-Setting mit Wandmontage-Option.
//
// Features:
//   - Korrekte Proportionen & Bracket-Geometrie
//   - Wandmontage-Toggle (90° Rotation)
//   - Wohnzimmer-Umgebung (Boden, Waende, Moebel)
//   - Interaktive Objekte (greifbar in Zukunft)
//   - Gelenk-Limits basierend auf mechanischen Grenzen
//
// Abhaengigkeiten: three.js (global: window.THREE)
// ============================================================

(function() {
    'use strict';

    var initialized = false;
    var scene, camera, renderer;
    var animFrame = null;

    // === Arm-Gruppen (kinematische Kette) ===
    var armRoot;         // Wurzel-Gruppe (fuer Wandmontage-Rotation)
    var baseGroup;       // Basis-Drehung (Y)
    var shoulderGroup;   // Schulter-Pitch (X)
    var upperArmGroup;   // Traegt Oberarm-Mesh
    var elbowGroup;      // Ellbogen-Pitch (X)
    var forearmGroup;    // Traegt Unterarm-Mesh
    var wristGroup;      // Handgelenk-Rotation (Y)
    var gripperLeft, gripperRight;

    // === Wandmontage ===
    var wallMounted = false;
    var WALL_MOUNT_HEIGHT = 14;  // Hoehe der Wandhalterung (140mm)

    // === Interaktive Objekte ===
    var interactiveObjects = [];

    // === Materialien ===
    var matAcrylic, matAcrylicDark, matServo, matServoHorn;
    var matBracket, matGripper, matJoint, matNotaus;
    // Raum-Materialien
    var matFloor, matWall, matWallAccent, matTable, matShelf;
    var matObjRed, matObjBlue, matObjYellow, matObjGreen;

    // === Massen (1 unit = 10mm) ===
    var BASE_W   = 10;
    var BASE_D   = 10;
    var BASE_H   = 1.5;
    var TURNTABLE_H = 0.8;
    var SHOULDER_H  = 6;
    var UPPER_ARM_L = 10.5;
    var FOREARM_L   = 9.5;
    var WRIST_L     = 3.0;
    var GRIPPER_L   = 5.5;
    var SERVO_W     = 2.3;
    var SERVO_H     = 1.2;
    var SERVO_D     = 4.0;
    var BRACKET_T   = 0.3;
    var BRACKET_W   = 3.5;
    var STANDOFF_R  = 0.15;

    // === Raum-Massen (geraeumiges Wohnzimmer) ===
    var ROOM_W = 160;  // 16m breit
    var ROOM_D = 120;  // 12m tief
    var ROOM_H = 55;   // 5.5m hoch (Altbau-Feeling)
    var TABLE_W = 30;
    var TABLE_D = 18;
    var TABLE_H = 8;   // 80cm Tischhoehe

    function createMaterials() {
        // --- Arm-Materialien ---
        matAcrylic = new THREE.MeshPhongMaterial({
            color: 0x1a1a2e, transparent: true, opacity: 0.85,
            shininess: 80, specular: 0x222244
        });
        matAcrylicDark = new THREE.MeshPhongMaterial({
            color: 0x0f0f1a, transparent: true, opacity: 0.9, shininess: 60
        });
        matServo = new THREE.MeshPhongMaterial({ color: 0x1a1a3a, shininess: 30 });
        matServoHorn = new THREE.MeshPhongMaterial({ color: 0xdddddd, shininess: 50 });
        matBracket = new THREE.MeshPhongMaterial({
            color: 0x555555, shininess: 100, specular: 0x888888
        });
        matGripper = new THREE.MeshPhongMaterial({ color: 0x10b981, shininess: 40 });
        matJoint = new THREE.MeshPhongMaterial({ color: 0xf59e0b, shininess: 80 });
        matNotaus = new THREE.MeshPhongMaterial({
            color: 0xff5000, emissive: 0x331000, shininess: 60
        });

        // --- Raum-Materialien ---
        matFloor = new THREE.MeshPhongMaterial({
            color: 0x6b4c3b, shininess: 40, specular: 0x2a1a10
        });
        matWall = new THREE.MeshPhongMaterial({
            color: 0x5a5a6a, shininess: 15
        });
        matWallAccent = new THREE.MeshPhongMaterial({
            color: 0x3a5a8f, shininess: 20
        });
        matTable = new THREE.MeshPhongMaterial({
            color: 0x6b5040, shininess: 50, specular: 0x332218
        });
        matShelf = new THREE.MeshPhongMaterial({
            color: 0x7a5a45, shininess: 30
        });

        // --- Interaktive Objekt-Materialien ---
        matObjRed = new THREE.MeshPhongMaterial({
            color: 0xe74c3c, shininess: 60, specular: 0x331111
        });
        matObjBlue = new THREE.MeshPhongMaterial({
            color: 0x3498db, shininess: 60, specular: 0x112233
        });
        matObjYellow = new THREE.MeshPhongMaterial({
            color: 0xf1c40f, shininess: 60, specular: 0x332200
        });
        matObjGreen = new THREE.MeshPhongMaterial({
            color: 0x2ecc71, shininess: 60, specular: 0x113311
        });
    }

    // ============================================================
    // GEOMETRIE-HELFER
    // ============================================================

    function makeBox(w, h, d, material) {
        return new THREE.Mesh(new THREE.BoxGeometry(w, h, d), material);
    }

    function makeCylinder(rTop, rBot, h, material, segments) {
        segments = segments || 16;
        return new THREE.Mesh(new THREE.CylinderGeometry(rTop, rBot, h, segments), material);
    }

    function makeServo() {
        var group = new THREE.Group();
        var body = makeBox(SERVO_W, SERVO_H, SERVO_D, matServo);
        group.add(body);
        var horn = makeCylinder(0.5, 0.5, 0.15, matServoHorn, 12);
        horn.position.y = SERVO_H / 2 + 0.08;
        group.add(horn);
        var axle = makeCylinder(0.2, 0.2, SERVO_H + 0.4, matBracket, 8);
        group.add(axle);
        return group;
    }

    function makeBracketPlate(w, h, thickness) {
        return makeBox(thickness, h, w, matAcrylic);
    }

    function makeDualBracket(w, h, separation, withStandoffs) {
        var group = new THREE.Group();
        var plate1 = makeBracketPlate(w, h, BRACKET_T);
        plate1.position.x = -separation / 2;
        group.add(plate1);

        var plate2 = makeBracketPlate(w, h, BRACKET_T);
        plate2.position.x = separation / 2;
        group.add(plate2);

        if (withStandoffs) {
            for (var i = 0; i < 2; i++) {
                var standoff = makeCylinder(STANDOFF_R, STANDOFF_R, separation - BRACKET_T, matBracket, 6);
                standoff.rotation.z = Math.PI / 2;
                standoff.position.set(0, -h * 0.35, (i === 0 ? -1 : 1) * w * 0.35);
                group.add(standoff);
            }
        }
        return group;
    }

    // ============================================================
    // WOHNZIMMER-UMGEBUNG
    // ============================================================

    function buildRoom() {
        var wallZ = -ROOM_D / 2; // Rueckwand Z-Position
        var matWallLight = new THREE.MeshPhongMaterial({ color: 0x6a6a78, shininess: 15 });
        var matWood = new THREE.MeshPhongMaterial({ color: 0x5a4030, shininess: 40, specular: 0x1a1008 });
        var matFabric = new THREE.MeshPhongMaterial({ color: 0x3a4a5a, shininess: 5 });
        var matCushion = new THREE.MeshPhongMaterial({ color: 0x4a5a6a, shininess: 8 });
        var matTV = new THREE.MeshPhongMaterial({ color: 0x111111, shininess: 90 });
        var matTVScreen = new THREE.MeshPhongMaterial({ color: 0x0a0a12, shininess: 120, specular: 0x222233 });
        var matPlankLine = new THREE.MeshPhongMaterial({ color: 0x4a3520, shininess: 20 });

        // ============ BODEN (Parkett) ============
        var floor = makeBox(ROOM_W, 0.5, ROOM_D, matFloor);
        floor.position.set(0, -0.25, 0);
        scene.add(floor);

        for (var i = -ROOM_W / 2 + 5; i < ROOM_W / 2; i += 5) {
            var plank = makeBox(0.1, 0.02, ROOM_D, matPlankLine);
            plank.position.set(i, 0.02, 0);
            scene.add(plank);
        }

        // ============ WAENDE ============
        // Rueckwand (Arm-Seite)
        var backWall = makeBox(ROOM_W, ROOM_H, 0.5, matWall);
        backWall.position.set(0, ROOM_H / 2, wallZ + 0.25);
        scene.add(backWall);

        // Sockelleisten an allen Waenden
        var matBaseboard = new THREE.MeshPhongMaterial({ color: 0x3a3a45, shininess: 30 });
        var bb1 = makeBox(ROOM_W, 1.5, 0.4, matBaseboard);
        bb1.position.set(0, 0.75, wallZ + 0.5);
        scene.add(bb1);
        var bb2 = makeBox(0.4, 1.5, ROOM_D, matBaseboard);
        bb2.position.set(-ROOM_W / 2 + 0.5, 0.75, 0);
        scene.add(bb2);
        var bb3 = makeBox(0.4, 1.5, ROOM_D, matBaseboard);
        bb3.position.set(ROOM_W / 2 - 0.5, 0.75, 0);
        scene.add(bb3);

        // Zierleiste (Stuck-Andeutung oben)
        var crown1 = makeBox(ROOM_W, 1, 1, matBaseboard);
        crown1.position.set(0, ROOM_H - 0.5, wallZ + 0.5);
        scene.add(crown1);

        // Linke Wand
        var leftWall = makeBox(0.5, ROOM_H, ROOM_D, matWall);
        leftWall.position.set(-ROOM_W / 2 + 0.25, ROOM_H / 2, 0);
        scene.add(leftWall);

        // Rechte Wand
        var rightWall = makeBox(0.5, ROOM_H, ROOM_D, matWallLight);
        rightWall.position.set(ROOM_W / 2 - 0.25, ROOM_H / 2, 0);
        scene.add(rightWall);

        // Decke
        var ceiling = new THREE.Mesh(
            new THREE.BoxGeometry(ROOM_W, 0.3, ROOM_D),
            new THREE.MeshPhongMaterial({ color: 0x555565, transparent: true, opacity: 0.5 })
        );
        ceiling.position.set(0, ROOM_H, 0);
        scene.add(ceiling);

        // ============ WANDHALTERUNG FUER ARM (an Rueckwand, leicht rechts) ============
        var armWallX = 15; // Arm leicht rechts von der Mitte
        var wallBracket = makeBox(16, 1.2, 12, matShelf);
        wallBracket.position.set(armWallX, WALL_MOUNT_HEIGHT, wallZ + 6.5);
        scene.add(wallBracket);

        var bracketL = makeBox(0.6, 6, 6, matBracket);
        bracketL.position.set(armWallX - 7, WALL_MOUNT_HEIGHT - 3, wallZ + 3.5);
        scene.add(bracketL);
        var bracketR = makeBox(0.6, 6, 6, matBracket);
        bracketR.position.set(armWallX + 7, WALL_MOUNT_HEIGHT - 3, wallZ + 3.5);
        scene.add(bracketR);

        // ============ ARBEITSTISCH (vor dem Arm) ============
        var tableZ = wallZ + 30;
        var tableTop = makeBox(TABLE_W, 0.8, TABLE_D, matTable);
        tableTop.position.set(armWallX, TABLE_H, tableZ);
        scene.add(tableTop);

        var legOffsets = [
            [-TABLE_W / 2 + 1.2, -TABLE_D / 2 + 1.2],
            [ TABLE_W / 2 - 1.2, -TABLE_D / 2 + 1.2],
            [-TABLE_W / 2 + 1.2,  TABLE_D / 2 - 1.2],
            [ TABLE_W / 2 - 1.2,  TABLE_D / 2 - 1.2]
        ];
        for (var l = 0; l < legOffsets.length; l++) {
            var leg = makeCylinder(0.5, 0.5, TABLE_H, matTable, 8);
            leg.position.set(armWallX + legOffsets[l][0], TABLE_H / 2, tableZ + legOffsets[l][1]);
            scene.add(leg);
        }

        // ============ TV-WOHNWAND (links an Rueckwand) ============
        var tvWallX = -35;

        // Sideboard / TV-Lowboard
        var lowboard = makeBox(40, 5, 8, matWood);
        lowboard.position.set(tvWallX, 2.5, wallZ + 4.5);
        scene.add(lowboard);

        // Regal-Module links und rechts vom TV
        var shelfUnit = makeBox(6, 28, 7, matWood);
        shelfUnit.position.set(tvWallX - 20, 14, wallZ + 4);
        scene.add(shelfUnit);

        var shelfUnit2 = makeBox(6, 28, 7, matWood);
        shelfUnit2.position.set(tvWallX + 20, 14, wallZ + 4);
        scene.add(shelfUnit2);

        // Regalboeden im linken Modul
        for (var s = 0; s < 4; s++) {
            var shelfBoard = makeBox(5.5, 0.3, 6.5, matWood);
            shelfBoard.position.set(tvWallX - 20, 4 + s * 7, wallZ + 4);
            scene.add(shelfBoard);
        }
        // Regalboeden im rechten Modul
        for (var s2 = 0; s2 < 4; s2++) {
            var shelfBoard2 = makeBox(5.5, 0.3, 6.5, matWood);
            shelfBoard2.position.set(tvWallX + 20, 4 + s2 * 7, wallZ + 4);
            scene.add(shelfBoard2);
        }

        // Buecher in den Regalen
        var bookColors = [0x8e3b3b, 0x3b5e8e, 0x3b8e5e, 0x8e7e3b, 0x6b3b8e, 0x8e5e3b, 0x3b8e8e, 0x8e6b3b];
        for (var bi = 0; bi < 3; bi++) {
            for (var bj = 0; bj < 4; bj++) {
                var bookH = 4 + Math.random() * 2;
                var book = makeBox(1.8, bookH, 5, new THREE.MeshPhongMaterial({
                    color: bookColors[(bi * 4 + bj) % bookColors.length], shininess: 20
                }));
                book.position.set(tvWallX - 22.5 + bj * 2, 4.5 + bi * 7 + bookH / 2, wallZ + 4);
                scene.add(book);
            }
        }

        // Deko-Objekte im rechten Regal
        var vase = makeCylinder(1, 0.8, 5, new THREE.MeshPhongMaterial({
            color: 0xc4956a, shininess: 60
        }), 12);
        vase.position.set(tvWallX + 20, 6.5, wallZ + 4);
        scene.add(vase);

        var decoSphere = new THREE.Mesh(
            new THREE.SphereGeometry(1.5, 12, 12),
            new THREE.MeshPhongMaterial({ color: 0x88aacc, shininess: 80 })
        );
        decoSphere.position.set(tvWallX + 19, 13, wallZ + 4);
        scene.add(decoSphere);

        // TV-Rueckwand (Paneel hinter dem TV)
        var tvPanel = makeBox(35, 22, 0.5, new THREE.MeshPhongMaterial({
            color: 0x2a2a32, shininess: 20
        }));
        tvPanel.position.set(tvWallX, 17, wallZ + 0.8);
        scene.add(tvPanel);

        // TV (55 Zoll ≈ 12x7 units)
        var tvBody = makeBox(24, 14, 0.8, matTV);
        tvBody.position.set(tvWallX, 18, wallZ + 1.5);
        scene.add(tvBody);

        var tvScreen = makeBox(23, 13, 0.1, matTVScreen);
        tvScreen.position.set(tvWallX, 18, wallZ + 2);
        scene.add(tvScreen);

        // TV-Standfuss
        var tvStand = makeBox(8, 0.5, 4, matTV);
        tvStand.position.set(tvWallX, 5.5, wallZ + 2.5);
        scene.add(tvStand);
        var tvNeck = makeBox(1.5, 5, 1.5, matTV);
        tvNeck.position.set(tvWallX, 8, wallZ + 2);
        scene.add(tvNeck);

        // ============ COUCH (Mitte des Raums, Blick zum TV = Richtung -Z) ============
        var couchX = -35;
        var couchZ = wallZ + 55;
        var couchW = 40;
        var couchD = 12;
        var couchSeatH = 5;
        var couchBackH = 10;

        // Sitzflaeche
        var couchSeat = makeBox(couchW, couchSeatH, couchD, matFabric);
        couchSeat.position.set(couchX, couchSeatH / 2, couchZ);
        scene.add(couchSeat);

        // Rueckenlehne (hintere Seite = +Z, weg vom TV)
        var couchBack = makeBox(couchW, couchBackH, 3, matFabric);
        couchBack.position.set(couchX, couchBackH / 2, couchZ + couchD / 2 + 1.5);
        scene.add(couchBack);

        // Armlehnen
        var armrestL = makeBox(3, 7, couchD + 3, matFabric);
        armrestL.position.set(couchX - couchW / 2 - 1.5, 3.5, couchZ + 1.5);
        scene.add(armrestL);
        var armrestR = makeBox(3, 7, couchD + 3, matFabric);
        armrestR.position.set(couchX + couchW / 2 + 1.5, 3.5, couchZ + 1.5);
        scene.add(armrestR);

        // Sitzkissen (2 Stueck, leicht erhoeht)
        var cushion1 = makeBox(18, 1.5, 10, matCushion);
        cushion1.position.set(couchX - 10, couchSeatH + 0.75, couchZ - 0.5);
        scene.add(cushion1);
        var cushion2 = makeBox(18, 1.5, 10, matCushion);
        cushion2.position.set(couchX + 10, couchSeatH + 0.75, couchZ - 0.5);
        scene.add(cushion2);

        // Rueckenkissen (an der Rueckenlehne, Richtung +Z)
        var backCushion1 = makeBox(12, 8, 3, new THREE.MeshPhongMaterial({
            color: 0x556677, shininess: 8
        }));
        backCushion1.position.set(couchX - 8, 8, couchZ + couchD / 2 - 1);
        backCushion1.rotation.x = 0.15;
        scene.add(backCushion1);
        var backCushion2 = makeBox(12, 8, 3, new THREE.MeshPhongMaterial({
            color: 0x556677, shininess: 8
        }));
        backCushion2.position.set(couchX + 8, 8, couchZ + couchD / 2 - 1);
        backCushion2.rotation.x = 0.15;
        scene.add(backCushion2);

        // Deko-Kissen (farbig, an der Rueckenlehne)
        var decoPillow = makeBox(5, 5, 2, new THREE.MeshPhongMaterial({
            color: 0xc97b3a, shininess: 10
        }));
        decoPillow.position.set(couchX - 16, 7, couchZ + 3);
        decoPillow.rotation.z = 0.2;
        scene.add(decoPillow);

        // Couchtisch (zwischen Couch und TV = Richtung -Z)
        var coffeeTableTop = makeBox(20, 0.8, 10, matWood);
        coffeeTableTop.position.set(couchX, 4.5, couchZ - 14);
        scene.add(coffeeTableTop);
        // Couchtisch-Beine
        var ctLegs = [[-8, -3.5], [8, -3.5], [-8, 3.5], [8, 3.5]];
        for (var ct = 0; ct < ctLegs.length; ct++) {
            var ctLeg = makeBox(1, 4, 1, matWood);
            ctLeg.position.set(couchX + ctLegs[ct][0], 2, couchZ - 14 + ctLegs[ct][1]);
            scene.add(ctLeg);
        }

        // Tasse auf Couchtisch
        var mug = makeCylinder(0.8, 0.8, 1.5, new THREE.MeshPhongMaterial({
            color: 0xeeeeee, shininess: 60
        }), 12);
        mug.position.set(couchX + 3, 5.65, couchZ - 14);
        scene.add(mug);

        // ============ STEHLAMPE (rechte hintere Ecke) ============
        var lampX = ROOM_W / 2 - 10;
        var lampZ = wallZ + 10;

        var lampBase2 = makeCylinder(3, 3, 0.6, matBracket, 16);
        lampBase2.position.set(lampX, 0.3, lampZ);
        scene.add(lampBase2);

        var lampPole2 = makeCylinder(0.4, 0.4, 22, matBracket, 8);
        lampPole2.position.set(lampX, 11, lampZ);
        scene.add(lampPole2);

        var lampShade2 = new THREE.Mesh(
            new THREE.CylinderGeometry(2, 5, 6, 16, 1, true),
            new THREE.MeshPhongMaterial({
                color: 0xf5e6c8, transparent: true, opacity: 0.7,
                side: THREE.DoubleSide, emissive: 0x665533, emissiveIntensity: 0.6
            })
        );
        lampShade2.position.set(lampX, 23, lampZ);
        scene.add(lampShade2);

        var lampLight2 = new THREE.PointLight(0xffe4b5, 1.5, 60);
        lampLight2.position.set(lampX, 22, lampZ);
        scene.add(lampLight2);

        // ============ TOPFPFLANZE (linke Ecke vorne) ============
        var plantX = -ROOM_W / 2 + 12;
        var plantZ = wallZ + 50;

        var bigPot = makeCylinder(3, 2.5, 5, new THREE.MeshPhongMaterial({
            color: 0x7a5a3a, shininess: 25
        }), 12);
        bigPot.position.set(plantX, 2.5, plantZ);
        scene.add(bigPot);

        // Erde
        var soil = makeCylinder(2.8, 2.8, 0.5, new THREE.MeshPhongMaterial({
            color: 0x3a2a1a, shininess: 5
        }), 12);
        soil.position.set(plantX, 5.25, plantZ);
        scene.add(soil);

        // Blaetter (mehrere Kugeln)
        var leafMat = new THREE.MeshPhongMaterial({ color: 0x2d8a4e, shininess: 15 });
        var leafPositions = [[0, 9, 0], [-1.5, 8, 1], [1.5, 8.5, -1], [0, 7, -1.5], [-1, 10, -0.5]];
        for (var lp = 0; lp < leafPositions.length; lp++) {
            var leaf = new THREE.Mesh(new THREE.SphereGeometry(2.5, 8, 6), leafMat);
            leaf.position.set(plantX + leafPositions[lp][0], leafPositions[lp][1], plantZ + leafPositions[lp][2]);
            leaf.scale.set(1, 0.8, 1);
            scene.add(leaf);
        }

        // Stamm
        var trunk = makeCylinder(0.5, 0.5, 5, new THREE.MeshPhongMaterial({
            color: 0x5a4030, shininess: 15
        }), 6);
        trunk.position.set(plantX, 7, plantZ);
        scene.add(trunk);

        // ============ BILDERRAHMEN an Rueckwand ============
        // Grosses Bild links neben Arm
        var frameOuter = makeBox(12, 9, 0.4, matBracket);
        frameOuter.position.set(armWallX - 25, 22, wallZ + 0.5);
        scene.add(frameOuter);
        var frameInner = makeBox(11, 8, 0.2, new THREE.MeshPhongMaterial({ color: 0x5a7aaa, shininess: 10 }));
        frameInner.position.set(armWallX - 25, 22, wallZ + 0.7);
        scene.add(frameInner);

        // Kleines Bild rechts
        var frame2Outer = makeBox(8, 10, 0.4, matBracket);
        frame2Outer.position.set(armWallX + 25, 24, wallZ + 0.5);
        scene.add(frame2Outer);
        var frame2Inner = makeBox(7, 9, 0.2, new THREE.MeshPhongMaterial({ color: 0x7a9a6a, shininess: 10 }));
        frame2Inner.position.set(armWallX + 25, 24, wallZ + 0.7);
        scene.add(frame2Inner);

        // Bild an linker Wand
        var frame3Outer = makeBox(0.4, 10, 14, matBracket);
        frame3Outer.position.set(-ROOM_W / 2 + 0.6, 25, wallZ + 40);
        scene.add(frame3Outer);
        var frame3Inner = makeBox(0.2, 9, 13, new THREE.MeshPhongMaterial({ color: 0xaa7a5a, shininess: 10 }));
        frame3Inner.position.set(-ROOM_W / 2 + 0.8, 25, wallZ + 40);
        scene.add(frame3Inner);

        // ============ TEPPICH (grosser Wohnzimmer-Teppich) ============
        var rug = makeBox(50, 0.15, 35, new THREE.MeshPhongMaterial({
            color: 0x5a4058, shininess: 5
        }));
        rug.position.set(couchX, 0.08, couchZ - 5);
        scene.add(rug);

        // Teppich-Rand (Muster-Andeutung)
        var rugBorder = makeBox(52, 0.1, 37, new THREE.MeshPhongMaterial({
            color: 0x6a506a, shininess: 5
        }));
        rugBorder.position.set(couchX, 0.05, couchZ - 5);
        scene.add(rugBorder);

        // ============ FENSTER-ANDEUTUNG (rechte Wand) ============
        var windowFrame = makeBox(0.6, 20, 25, matBaseboard);
        windowFrame.position.set(ROOM_W / 2 - 0.6, 22, wallZ + 40);
        scene.add(windowFrame);
        var windowGlass = makeBox(0.2, 18, 23, new THREE.MeshPhongMaterial({
            color: 0x8ab4d4, transparent: true, opacity: 0.3, shininess: 100
        }));
        windowGlass.position.set(ROOM_W / 2 - 0.4, 22, wallZ + 40);
        scene.add(windowGlass);

        // Fenster-Licht (simuliert Tageslicht von aussen)
        var windowLight = new THREE.PointLight(0xddeeff, 2.0, 80);
        windowLight.position.set(ROOM_W / 2 - 5, 25, wallZ + 40);
        scene.add(windowLight);
    }

    // ============================================================
    // INTERAKTIVE OBJEKTE (auf dem Tisch)
    // ============================================================

    function buildInteractiveObjects() {
        var armX = 15; // Gleiche X-Position wie Arm
        var tableY = TABLE_H + 0.4;
        var tableZ = -ROOM_D / 2 + 30; // Gleiche Position wie Tisch in buildRoom()

        // Objekte verteilt auf dem Tisch
        var cube = makeBox(2.5, 2.5, 2.5, matObjRed);
        cube.position.set(armX - 8, tableY + 1.25, tableZ - 2);
        cube.userData.interactive = true;
        cube.userData.name = 'Roter Wuerfel';
        scene.add(cube);
        interactiveObjects.push(cube);

        var sphere = new THREE.Mesh(
            new THREE.SphereGeometry(1.5, 20, 20), matObjBlue
        );
        sphere.position.set(armX, tableY + 1.5, tableZ);
        sphere.userData.interactive = true;
        sphere.userData.name = 'Blaue Kugel';
        scene.add(sphere);
        interactiveObjects.push(sphere);

        var cylinder = makeCylinder(1, 1, 3, matObjYellow, 16);
        cylinder.position.set(armX + 8, tableY + 1.5, tableZ + 3);
        cylinder.userData.interactive = true;
        cylinder.userData.name = 'Gelber Zylinder';
        scene.add(cylinder);
        interactiveObjects.push(cylinder);

        var pyramid = new THREE.Mesh(
            new THREE.ConeGeometry(1.5, 3, 4), matObjGreen
        );
        pyramid.position.set(armX - 3, tableY + 1.5, tableZ + 4);
        pyramid.userData.interactive = true;
        pyramid.userData.name = 'Gruene Pyramide';
        scene.add(pyramid);
        interactiveObjects.push(pyramid);

        var torusGeo = new THREE.TorusGeometry(1, 0.3, 8, 12);
        var gear = new THREE.Mesh(torusGeo, matBracket);
        gear.position.set(armX + 5, tableY + 1, tableZ - 4);
        gear.rotation.x = Math.PI / 2;
        gear.userData.interactive = true;
        gear.userData.name = 'Zahnrad';
        scene.add(gear);
        interactiveObjects.push(gear);

        var pen = makeCylinder(0.2, 0.2, 5, new THREE.MeshPhongMaterial({
            color: 0x2c3e50, shininess: 80
        }), 8);
        pen.rotation.z = Math.PI / 2;
        pen.position.set(armX + 2, tableY + 0.2, tableZ + 2);
        pen.userData.interactive = true;
        pen.userData.name = 'Stift';
        scene.add(pen);
        interactiveObjects.push(pen);
    }

    // ============================================================
    // ARM AUFBAUEN
    // ============================================================

    function buildArm() {
        // Wurzel-Gruppe fuer Wandmontage-Rotation
        armRoot = new THREE.Group();
        // Standard: Arm steht auf dem Wand-Regal (leicht rechts)
        armRoot.position.set(15, WALL_MOUNT_HEIGHT + 0.5, -ROOM_D / 2 + 6.5);
        scene.add(armRoot);

        // === BODEN-PLATTFORM ===
        var basePlatform = makeBox(BASE_W, BASE_H, BASE_D, matAcrylicDark);
        basePlatform.position.y = BASE_H / 2;
        armRoot.add(basePlatform);

        // Logo-Bereich
        var logoPlate = makeCylinder(3.5, 4, 0.3, matAcrylic, 32);
        logoPlate.position.y = BASE_H + 0.15;
        armRoot.add(logoPlate);

        // === DREHBARE BASIS (Servo 1: D9) ===
        baseGroup = new THREE.Group();
        baseGroup.position.y = BASE_H + 0.3;
        armRoot.add(baseGroup);

        var turntable = makeCylinder(3, 3.2, TURNTABLE_H, matBracket, 32);
        turntable.position.y = TURNTABLE_H / 2;
        baseGroup.add(turntable);

        var baseServo = makeServo();
        baseServo.rotation.x = Math.PI / 2;
        baseServo.position.set(0, -0.3, 0);
        baseServo.visible = false;
        baseGroup.add(baseServo);

        // === SCHULTER-BRACKET (Servo 2: D6) ===
        var shoulderBracket = new THREE.Group();
        shoulderBracket.position.y = TURNTABLE_H;
        baseGroup.add(shoulderBracket);

        var sPlateL = makeBracketPlate(BRACKET_W, SHOULDER_H, BRACKET_T);
        sPlateL.position.set(-2, SHOULDER_H / 2, 0);
        shoulderBracket.add(sPlateL);

        var sPlateR = makeBracketPlate(BRACKET_W, SHOULDER_H, BRACKET_T);
        sPlateR.position.set(2, SHOULDER_H / 2, 0);
        shoulderBracket.add(sPlateR);

        var sBack = makeBox(4 + BRACKET_T, SHOULDER_H * 0.6, BRACKET_T, matAcrylic);
        sBack.position.set(0, SHOULDER_H * 0.3, -BRACKET_W / 2);
        shoulderBracket.add(sBack);

        var shoulderServo = makeServo();
        shoulderServo.rotation.z = Math.PI / 2;
        shoulderServo.position.set(0, SHOULDER_H, 0);
        shoulderBracket.add(shoulderServo);

        var shoulderAxle = makeCylinder(0.25, 0.25, 4.5, matJoint, 12);
        shoulderAxle.rotation.z = Math.PI / 2;
        shoulderAxle.position.set(0, SHOULDER_H, 0);
        shoulderBracket.add(shoulderAxle);

        // === SCHULTER PIVOT ===
        shoulderGroup = new THREE.Group();
        shoulderGroup.position.set(0, SHOULDER_H, 0);
        shoulderBracket.add(shoulderGroup);

        // === OBERARM ===
        upperArmGroup = new THREE.Group();
        shoulderGroup.add(upperArmGroup);

        var upperArm = makeDualBracket(BRACKET_W, UPPER_ARM_L, 3.5, true);
        upperArm.position.y = UPPER_ARM_L / 2;
        upperArmGroup.add(upperArm);

        var elbowServo = makeServo();
        elbowServo.rotation.z = Math.PI / 2;
        elbowServo.position.set(0, UPPER_ARM_L, 0);
        upperArmGroup.add(elbowServo);

        var elbowAxle = makeCylinder(0.25, 0.25, 4.0, matJoint, 12);
        elbowAxle.rotation.z = Math.PI / 2;
        elbowAxle.position.set(0, UPPER_ARM_L, 0);
        upperArmGroup.add(elbowAxle);

        // === ELLBOGEN PIVOT ===
        elbowGroup = new THREE.Group();
        elbowGroup.position.y = UPPER_ARM_L;
        upperArmGroup.add(elbowGroup);

        // === UNTERARM ===
        forearmGroup = new THREE.Group();
        elbowGroup.add(forearmGroup);

        var forearm = makeDualBracket(BRACKET_W - 0.5, FOREARM_L, 3.0, true);
        forearm.position.y = FOREARM_L / 2;
        forearmGroup.add(forearm);

        var wristServo = makeServo();
        wristServo.rotation.z = Math.PI / 2;
        wristServo.position.set(0, FOREARM_L, 0);
        forearmGroup.add(wristServo);

        var wristAxle = makeCylinder(0.2, 0.2, 3.5, matJoint, 12);
        wristAxle.rotation.z = Math.PI / 2;
        wristAxle.position.set(0, FOREARM_L, 0);
        forearmGroup.add(wristAxle);

        // === HANDGELENK PIVOT ===
        wristGroup = new THREE.Group();
        wristGroup.position.y = FOREARM_L;
        forearmGroup.add(wristGroup);

        var wristAdapter = makeBox(2.5, WRIST_L, 2.5, matAcrylic);
        wristAdapter.position.y = WRIST_L / 2;
        wristGroup.add(wristAdapter);

        // === GREIFER ===
        var gripperBase = new THREE.Group();
        gripperBase.position.y = WRIST_L;
        wristGroup.add(gripperBase);

        var gripperHousing = makeBox(3, 1.5, 2.5, matAcrylic);
        gripperHousing.position.y = 0.75;
        gripperBase.add(gripperHousing);

        var gripperServo = makeServo();
        gripperServo.scale.set(0.7, 0.7, 0.7);
        gripperServo.position.set(0, 0.5, 0);
        gripperBase.add(gripperServo);

        gripperLeft = new THREE.Group();
        gripperLeft.position.set(-0.8, 1.5, 0);
        gripperBase.add(gripperLeft);

        var fingerL = makeBox(0.4, GRIPPER_L, 1.0, matGripper);
        fingerL.position.y = GRIPPER_L / 2;
        gripperLeft.add(fingerL);

        var tipL = makeBox(0.4, 1.2, 1.0, matGripper);
        tipL.position.set(0.3, GRIPPER_L, 0);
        tipL.rotation.z = -0.3;
        gripperLeft.add(tipL);

        gripperRight = new THREE.Group();
        gripperRight.position.set(0.8, 1.5, 0);
        gripperBase.add(gripperRight);

        var fingerR = makeBox(0.4, GRIPPER_L, 1.0, matGripper);
        fingerR.position.y = GRIPPER_L / 2;
        gripperRight.add(fingerR);

        var tipR = makeBox(0.4, 1.2, 1.0, matGripper);
        tipR.position.set(-0.3, GRIPPER_L, 0);
        tipR.rotation.z = 0.3;
        gripperRight.add(tipR);
    }

    // ============================================================
    // WANDMONTAGE-TOGGLE
    // ============================================================

    function setWallMountMode(mounted) {
        wallMounted = mounted;
        if (!armRoot) return;

        if (mounted) {
            // Arm 90° nach vorne kippen → haengt von der Wand, zeigt in den Raum (+Z)
            armRoot.rotation.x = Math.PI / 2;
            armRoot.position.set(15, WALL_MOUNT_HEIGHT + 8, -ROOM_D / 2 + 2);
        } else {
            // Standard: aufrecht stehend auf dem Regal
            armRoot.rotation.x = 0;
            armRoot.position.set(15, WALL_MOUNT_HEIGHT + 0.5, -ROOM_D / 2 + 6.5);
        }

        // Button-Text aktualisieren
        var btn = document.getElementById('ra-3d-toggle-mount');
        if (btn) {
            var label = mounted ? 'Tischmontage' : 'Wandmontage';
            // SVG + Text
            btn.innerHTML = '<svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">' +
                (mounted
                    ? '<rect x="3" y="3" width="18" height="18" rx="2"/><path d="M12 8v8"/><path d="M8 16h8"/>'
                    : '<path d="M3 3h18v18H3z"/><path d="M12 8v8"/><path d="M8 12h8"/>'
                ) + '</svg> ' + label;
        }
    }

    // ============================================================
    // SZENE ERSTELLEN
    // ============================================================

    function initScene() {
        var container = document.getElementById('ra-3d-container');
        if (!container) return false;

        scene = new THREE.Scene();
        scene.background = new THREE.Color(0x1a1e2e);

        // Nebel weit weg (nur Raum-Raender)
        scene.fog = new THREE.Fog(0x1a1e2e, 150, 250);

        // Kamera
        camera = new THREE.PerspectiveCamera(
            45, container.clientWidth / container.clientHeight, 0.1, 300
        );

        // --- Beleuchtung (helles Wohnzimmer, Tageslicht-Atmosphaere) ---
        var ambient = new THREE.AmbientLight(0x8888aa, 2.5);
        scene.add(ambient);

        // Hemisphere-Licht (Himmel oben warm, Boden kuehl)
        var hemi = new THREE.HemisphereLight(0xffeebb, 0x404060, 1.5);
        scene.add(hemi);

        // Haupt-Deckenleuchte Mitte
        var ceilingLight = new THREE.PointLight(0xffeedd, 2.0, 120);
        ceilingLight.position.set(0, ROOM_H - 2, -ROOM_D / 2 + 40);
        scene.add(ceilingLight);

        // Zweite Deckenleuchte vorne
        var ceilingLight2 = new THREE.PointLight(0xffeedd, 1.5, 100);
        ceilingLight2.position.set(0, ROOM_H - 2, 10);
        scene.add(ceilingLight2);

        // Dritte Deckenleuchte hinten ueber dem Arm
        var ceilingLight3 = new THREE.PointLight(0xffffff, 1.5, 80);
        ceilingLight3.position.set(0, ROOM_H - 2, -ROOM_D / 2 + 10);
        scene.add(ceilingLight3);

        // Key Light von oben-vorne
        var keyLight = new THREE.DirectionalLight(0xffffff, 1.5);
        keyLight.position.set(10, 30, 20);
        scene.add(keyLight);

        // Warmes Fuell-Licht von rechts
        var fillLight = new THREE.DirectionalLight(0xffe4b5, 0.8);
        fillLight.position.set(30, 15, 10);
        scene.add(fillLight);

        // Fuell-Licht von links
        var fillLight2 = new THREE.DirectionalLight(0xddeeff, 0.6);
        fillLight2.position.set(-30, 15, 15);
        scene.add(fillLight2);

        // Rim-Light von hinten
        var rimLight = new THREE.DirectionalLight(0x6688cc, 0.4);
        rimLight.position.set(0, 15, -40);
        scene.add(rimLight);

        // Renderer
        renderer = new THREE.WebGLRenderer({antialias: true, alpha: false});
        renderer.setPixelRatio(Math.min(window.devicePixelRatio, 2));
        renderer.setSize(container.clientWidth, container.clientHeight);
        container.appendChild(renderer.domElement);

        // Materialien + Umgebung + Arm + Objekte
        createMaterials();
        buildRoom();
        buildArm();
        buildInteractiveObjects();

        // Maus-Steuerung
        initSimpleControls(container);

        // Resize
        window.addEventListener('resize', function() {
            if (!container || !camera || !renderer) return;
            camera.aspect = container.clientWidth / container.clientHeight;
            camera.updateProjectionMatrix();
            renderer.setSize(container.clientWidth, container.clientHeight);
        });

        return true;
    }

    // ============================================================
    // MAUS-STEUERUNG
    // ============================================================

    var camTheta = Math.PI * 0.25;
    var camPhi   = Math.PI / 5.5;
    var camDist  = 85;
    var camTarget = {x: 5, y: 15, z: -20};
    var dragging = false, panning = false;
    var lastMouse = {x: 0, y: 0};

    // Default-Kamera Werte speichern
    var CAM_DEFAULTS = {
        theta: Math.PI * 0.25,
        phi: Math.PI / 5.5,
        dist: 85,
        target: {x: 5, y: 15, z: -20}
    };

    function initSimpleControls(container) {
        container.addEventListener('mousedown', function(e) {
            if (e.button === 0) dragging = true;
            if (e.button === 2) panning = true;
            lastMouse.x = e.clientX;
            lastMouse.y = e.clientY;
        });

        container.addEventListener('mousemove', function(e) {
            var dx = e.clientX - lastMouse.x;
            var dy = e.clientY - lastMouse.y;
            lastMouse.x = e.clientX;
            lastMouse.y = e.clientY;

            if (dragging) {
                camTheta -= dx * 0.005;
                camPhi   -= dy * 0.005;
                camPhi = Math.max(0.05, Math.min(Math.PI / 2 - 0.05, camPhi));
            }
            if (panning) {
                camTarget.x -= dx * 0.08;
                camTarget.y += dy * 0.08;
            }
        });

        container.addEventListener('mouseup', function() { dragging = false; panning = false; });
        container.addEventListener('mouseleave', function() { dragging = false; panning = false; });
        container.addEventListener('wheel', function(e) {
            e.preventDefault();
            camDist += e.deltaY * 0.03;
            camDist = Math.max(15, Math.min(200, camDist));
        }, {passive: false});
        container.addEventListener('contextmenu', function(e) { e.preventDefault(); });
    }

    function updateCamera() {
        if (!camera) return;
        var x = camTarget.x + camDist * Math.sin(camPhi) * Math.cos(camTheta);
        var y = camTarget.y + camDist * Math.cos(camPhi);
        var z = camTarget.z + camDist * Math.sin(camPhi) * Math.sin(camTheta);

        // Kamera innerhalb der Raum-Grenzen halten (mit Puffer)
        var margin = 3;
        x = Math.max(-ROOM_W / 2 + margin, Math.min(ROOM_W / 2 - margin, x));
        y = Math.max(1, Math.min(ROOM_H - margin, y));
        z = Math.max(-ROOM_D / 2 + margin, Math.min(ROOM_D / 2 - margin, z));

        camera.position.set(x, y, z);
        // Auch Target clampen damit man nicht durch die Wand schaut
        var tx = Math.max(-ROOM_W / 2 + margin, Math.min(ROOM_W / 2 - margin, camTarget.x));
        var ty = Math.max(0, Math.min(ROOM_H - margin, camTarget.y));
        var tz = Math.max(-ROOM_D / 2 + margin, Math.min(ROOM_D / 2 - margin, camTarget.z));
        camera.lookAt(tx, ty, tz);
    }

    // ============================================================
    // ARM-POSE + GELENK-LIMITS
    // ============================================================

    function deg2rad(d) { return d * Math.PI / 180; }

    var JOINT_LIMITS = {
        base:     { min: deg2rad(-90), max: deg2rad(90) },
        shoulder: { min: deg2rad(-5),  max: deg2rad(80) },    // +X = nach vorne, kaum nach hinten
        elbow:    { min: deg2rad(-5),  max: deg2rad(85) },    // +X = nach vorne/unten
        wrist:    { min: deg2rad(-90), max: deg2rad(90) },
        gripper:  { min: 0.0,          max: 0.6 }
    };

    function clampJoint(value, joint) {
        var lim = JOINT_LIMITS[joint];
        return Math.max(lim.min, Math.min(lim.max, value));
    }

    function updateArmPose() {
        var imu = window._raLastImu;
        if (!imu || !imu.s || imu.s.length < 3) return;

        if (baseGroup) {
            baseGroup.rotation.y = clampJoint(deg2rad(imu.s[2].h), 'base');
        }
        if (shoulderGroup) {
            shoulderGroup.rotation.x = clampJoint(deg2rad(imu.s[2].p), 'shoulder');
        }
        if (elbowGroup) {
            elbowGroup.rotation.x = clampJoint(deg2rad(imu.s[1].p - imu.s[2].p), 'elbow');
        }
        if (wristGroup) {
            wristGroup.rotation.y = clampJoint(deg2rad(imu.s[0].r), 'wrist');
        }
        if (gripperLeft && gripperRight && typeof imu.f === 'number') {
            var flex = Math.max(0, Math.min(100, imu.f));
            var angle = clampJoint((1 - flex / 100) * 0.6, 'gripper');
            gripperLeft.rotation.z = angle;
            gripperRight.rotation.z = -angle;
        }
        updateNotausVisuals(imu.notaus === true);
    }

    var currentNotaus = null;

    function updateNotausVisuals(active) {
        if (active === currentNotaus) return;
        currentNotaus = active;
        if (!armRoot) return;

        if (active) {
            armRoot.traverse(function(child) {
                if (child.isMesh && child.material !== matServo && child.material !== matServoHorn) {
                    if (!child.userData._origMat) {
                        child.userData._origMat = child.material;
                    }
                    child.material = matNotaus;
                }
            });
        } else {
            armRoot.traverse(function(child) {
                if (child.isMesh && child.userData._origMat) {
                    child.material = child.userData._origMat;
                }
            });
        }
    }

    // ============================================================
    // IDLE-ANIMATION
    // ============================================================

    function idleAnimation(time) {
        // Kein Idle — Arm steht aufrecht in Neutralstellung (alle Gelenke 0°)
        // Bewegt sich nur bei echten IMU-Daten via updateArmPose()
        if (window._raLastImu) return;

        if (baseGroup)     baseGroup.rotation.y    = 0;
        if (shoulderGroup) shoulderGroup.rotation.x = 0;
        if (elbowGroup)    elbowGroup.rotation.x   = 0;
        if (wristGroup)    wristGroup.rotation.y   = 0;
        if (gripperLeft)   gripperLeft.rotation.z   = 0.3; // Greifer leicht offen
        if (gripperRight)  gripperRight.rotation.z  = -0.3;
    }

    // ============================================================
    // KOLLISIONSERKENNUNG
    // ============================================================
    // Prueft ob Arm-Gelenke durch Boden, Waende oder Tisch gehen
    // und reduziert Schulter/Ellbogen-Winkel bis keine Kollision.

    var _collVec = new THREE.Vector3();
    var _collVec2 = new THREE.Vector3();
    var ARM_X = 15; // Arm-Position (muss mit buildRoom armWallX uebereinstimmen)

    function getWorldPos(obj) {
        obj.getWorldPosition(_collVec);
        return { x: _collVec.x, y: _collVec.y, z: _collVec.z };
    }

    // Punkt zwischen zwei Welt-Positionen interpolieren
    function lerpWorldPos(objA, objB, t) {
        objA.getWorldPosition(_collVec);
        objB.getWorldPosition(_collVec2);
        return {
            x: _collVec.x + (_collVec2.x - _collVec.x) * t,
            y: _collVec.y + (_collVec2.y - _collVec.y) * t,
            z: _collVec.z + (_collVec2.z - _collVec.z) * t
        };
    }

    function checkCollisionAtPoint(p) {
        // Boden
        if (p.y < 0.5) return true;
        // Rueckwand
        if (p.z < -ROOM_D / 2 + 2.5) return true;
        // Linke Wand
        if (p.x < -ROOM_W / 2 + 2) return true;
        // Rechte Wand
        if (p.x > ROOM_W / 2 - 2) return true;
        // Decke
        if (p.y > ROOM_H - 2) return true;
        // Arbeitstisch (AABB mit Puffer)
        var tZ = -ROOM_D / 2 + 30;
        if (p.x > ARM_X - TABLE_W / 2 - 2 && p.x < ARM_X + TABLE_W / 2 + 2 &&
            p.z > tZ - TABLE_D / 2 - 2 && p.z < tZ + TABLE_D / 2 + 2 &&
            p.y < TABLE_H + 2 && p.y > 0) {
            return true;
        }
        // Wandhalterung / Regal (Arm-Basis)
        if (p.x > ARM_X - 9 && p.x < ARM_X + 9 &&
            p.z < -ROOM_D / 2 + 7.5 &&
            p.y < WALL_MOUNT_HEIGHT + 1 && p.y > WALL_MOUNT_HEIGHT - 7) {
            return true;
        }
        return false;
    }

    function hasArmCollision() {
        armRoot.updateMatrixWorld(true);

        // Direkte Gelenkpunkte pruefen
        var joints = [shoulderGroup, elbowGroup, wristGroup, gripperLeft, gripperRight];
        for (var i = 0; i < joints.length; i++) {
            if (!joints[i]) continue;
            var p = getWorldPos(joints[i]);
            if (checkCollisionAtPoint(p)) return true;
        }

        // Zwischenpunkte entlang der Segmente pruefen (25%, 50%, 75%)
        var segments = [
            [shoulderGroup, elbowGroup],   // Oberarm
            [elbowGroup, wristGroup],      // Unterarm
            [wristGroup, gripperLeft]      // Handgelenk→Greifer
        ];
        for (var s = 0; s < segments.length; s++) {
            if (!segments[s][0] || !segments[s][1]) continue;
            for (var t = 0.25; t <= 0.75; t += 0.25) {
                var mp = lerpWorldPos(segments[s][0], segments[s][1], t);
                if (checkCollisionAtPoint(mp)) return true;
            }
        }

        return false;
    }

    function resolveCollisions() {
        if (!armRoot || !shoulderGroup || !elbowGroup) return;

        // Speichere gewuenschte Winkel
        var targetShoulder = shoulderGroup.rotation.x;
        var targetElbow = elbowGroup.rotation.x;

        // Schnell-Check: keine Kollision? Fertig.
        if (!hasArmCollision()) return;

        // Binaere Suche: finde maximalen sicheren Skalierungsfaktor (0..1)
        var lo = 0.0, hi = 1.0;
        for (var iter = 0; iter < 12; iter++) {
            var mid = (lo + hi) / 2;
            shoulderGroup.rotation.x = targetShoulder * mid;
            elbowGroup.rotation.x = targetElbow * mid;
            armRoot.updateMatrixWorld(true);

            if (hasArmCollision()) {
                hi = mid;
            } else {
                lo = mid;
            }
        }
        // Sicheren Wert verwenden (lo = letzter kollisionsfreier Faktor)
        shoulderGroup.rotation.x = targetShoulder * lo;
        elbowGroup.rotation.x = targetElbow * lo;
    }

    // ============================================================
    // RENDER-LOOP
    // ============================================================

    function animate(time) {
        animFrame = requestAnimationFrame(animate);
        updateCamera();
        idleAnimation(time || 0);
        updateArmPose();
        resolveCollisions();
        if (renderer && scene && camera) {
            renderer.render(scene, camera);
        }
    }

    // ============================================================
    // PUBLIC API
    // ============================================================

    window.raInit3D = function() {
        if (initialized) return;
        if (typeof THREE === 'undefined') {
            console.warn('[RA-3D] Three.js nicht geladen');
            return;
        }

        console.log('[RA-3D] Initialisiere Adeept 5-DOF Arm + Wohnzimmer...');
        if (initScene()) {
            initialized = true;
            animate();
            console.log('[RA-3D] 3D-Szene bereit');
        }
    };

    window.raDestroy3D = function() {
        if (animFrame) cancelAnimationFrame(animFrame);
        if (renderer) {
            renderer.dispose();
            var container = document.getElementById('ra-3d-container');
            if (container && renderer.domElement.parentNode === container) {
                container.removeChild(renderer.domElement);
            }
        }
        initialized = false;
    };

    window.raToggleWallMount = function() {
        setWallMountMode(!wallMounted);
    };

    window.raResetCam = function() {
        camTheta = CAM_DEFAULTS.theta;
        camPhi = CAM_DEFAULTS.phi;
        camDist = CAM_DEFAULTS.dist;
        camTarget.x = CAM_DEFAULTS.target.x;
        camTarget.y = CAM_DEFAULTS.target.y;
        camTarget.z = CAM_DEFAULTS.target.z;
    };

    console.log('[RA-3D] Adeept 5-DOF Arm + Wohnzimmer Modul geladen (lazy init)');

})();
