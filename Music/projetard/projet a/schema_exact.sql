-- ============================================================
-- BASE DE DONNÉES COMPLÈTE - FISHTECH PORT MARITIME
-- Colonnes EXACTES utilisées dans le code C++
-- ============================================================

-- ── SUPPRESSION ─────────────────────────────────────────────
DROP TABLE AFFECTATION   CASCADE CONSTRAINTS PURGE;
DROP TABLE CAPTURE       CASCADE CONSTRAINTS PURGE;
DROP TABLE NAVIRE_ARCHIVE CASCADE CONSTRAINTS PURGE;
DROP TABLE NAVIRE        CASCADE CONSTRAINTS PURGE;
DROP TABLE EMPLOYE       CASCADE CONSTRAINTS PURGE;
DROP TABLE QUAI          CASCADE CONSTRAINTS PURGE;

DROP SEQUENCE SEQ_QUAI;
DROP SEQUENCE SEQ_NAVIRE;
DROP SEQUENCE SEQ_CAPTURE;
DROP SEQUENCE SEQ_AFFECTATION;
DROP SEQUENCE SEQ_EMPLOYE;

-- ── SÉQUENCES ────────────────────────────────────────────────
CREATE SEQUENCE SEQ_QUAI        START WITH 1 INCREMENT BY 1 NOCACHE;
CREATE SEQUENCE SEQ_NAVIRE      START WITH 1 INCREMENT BY 1 NOCACHE;
CREATE SEQUENCE SEQ_CAPTURE     START WITH 1 INCREMENT BY 1 NOCACHE;
CREATE SEQUENCE SEQ_AFFECTATION START WITH 1 INCREMENT BY 1 NOCACHE;
CREATE SEQUENCE SEQ_EMPLOYE     START WITH 1 INCREMENT BY 1 NOCACHE;

-- ── TABLE QUAI ───────────────────────────────────────────────
-- Colonnes utilisées : id_quai, nom, places_total, navires_presents,
--                      adresse, etat, tirant_eau_max, equipements,
--                      date_debut, date_fin
CREATE TABLE QUAI (
    ID_QUAI          NUMBER         NOT NULL,
    NOM              VARCHAR2(100)  NOT NULL,
    PLACES_TOTAL     NUMBER         DEFAULT 0,
    NAVIRES_PRESENTS NUMBER         DEFAULT 0,
    ADRESSE          VARCHAR2(200),
    ETAT             VARCHAR2(50)   DEFAULT 'Disponible',
    TIRANT_EAU_MAX   NUMBER(10,2)   DEFAULT 10.0,
    EQUIPEMENTS      VARCHAR2(500),
    DATE_DEBUT       DATE           DEFAULT SYSDATE,
    DATE_FIN         DATE,
    CONSTRAINT PK_QUAI PRIMARY KEY (ID_QUAI)
);

-- ── TABLE NAVIRE ─────────────────────────────────────────────
-- Colonnes utilisées : id_navire, nom, immatriculation, type,
--                      capacite, statut
-- Valeurs statut : 'a quai' | 'en mer' | 'interdit'
CREATE TABLE NAVIRE (
    ID_NAVIRE        NUMBER         NOT NULL,
    NOM              VARCHAR2(100)  NOT NULL,
    IMMATRICULATION  VARCHAR2(100),
    TYPE             VARCHAR2(100),
    CAPACITE         NUMBER         DEFAULT 0,
    STATUT           VARCHAR2(50)   DEFAULT 'a quai',
    CONSTRAINT PK_NAVIRE PRIMARY KEY (ID_NAVIRE)
);

-- ── TABLE NAVIRE_ARCHIVE ─────────────────────────────────────
-- Colonnes utilisées : id_navire, nom, immatriculation, type,
--                      capacite, statut
CREATE TABLE NAVIRE_ARCHIVE (
    ID_NAVIRE        NUMBER         NOT NULL,
    NOM              VARCHAR2(100),
    IMMATRICULATION  VARCHAR2(100),
    TYPE             VARCHAR2(100),
    CAPACITE         NUMBER,
    STATUT           VARCHAR2(100),
    CONSTRAINT PK_NAVIRE_ARCHIVE PRIMARY KEY (ID_NAVIRE)
);

-- ── TABLE EMPLOYE ────────────────────────────────────────────
-- Colonnes utilisées : id, nom, poste, email, telephone, salaire
CREATE TABLE EMPLOYE (
    ID               NUMBER         NOT NULL,
    NOM              VARCHAR2(100)  NOT NULL,
    POSTE            VARCHAR2(100),
    EMAIL            VARCHAR2(150),
    TELEPHONE        VARCHAR2(20),
    SALAIRE          NUMBER(10,2),
    CONSTRAINT PK_EMPLOYE PRIMARY KEY (ID)
);

-- ── TABLE CAPTURE ────────────────────────────────────────────
-- Colonnes utilisées : ID_CAPTURE, ID_NAVIRE, DATE_CAPTURE,
--                      TYPE_POISSON, QUANTITE, AGENT, STATUT
-- Valeurs statut : 'Enregistre'|'Valide'|'En transit'|'Livre'|'En attente'
CREATE TABLE CAPTURE (
    ID_CAPTURE       NUMBER         NOT NULL,
    ID_NAVIRE        NUMBER,
    DATE_CAPTURE     DATE           DEFAULT SYSDATE,
    TYPE_POISSON     VARCHAR2(100),
    QUANTITE         NUMBER,
    AGENT            VARCHAR2(100),
    STATUT           VARCHAR2(50)   DEFAULT 'Enregistre',
    CONSTRAINT PK_CAPTURE PRIMARY KEY (ID_CAPTURE),
    CONSTRAINT FK_CAPTURE_NAVIRE FOREIGN KEY (ID_NAVIRE)
        REFERENCES NAVIRE(ID_NAVIRE) ON DELETE SET NULL
);

-- ── TABLE AFFECTATION ────────────────────────────────────────
-- Colonnes utilisées : navire_nom, tirant_navire, priorite,
--                      quai_id, date_arrivee, duree_estimee,
--                      score_final, statut
CREATE TABLE AFFECTATION (
    ID               NUMBER         NOT NULL,
    NAVIRE_NOM       VARCHAR2(100),
    TIRANT_NAVIRE    NUMBER(5,2),
    PRIORITE         NUMBER         DEFAULT 1,
    QUAI_ID          NUMBER,
    DATE_ARRIVEE     DATE,
    DUREE_ESTIMEE    NUMBER,
    SCORE_FINAL      NUMBER(5,2),
    STATUT           VARCHAR2(50)   DEFAULT 'En attente',
    CONSTRAINT PK_AFFECTATION PRIMARY KEY (ID),
    CONSTRAINT FK_AFFECTATION_QUAI FOREIGN KEY (QUAI_ID)
        REFERENCES QUAI(ID_QUAI) ON DELETE SET NULL
);

-- ── INDEX ────────────────────────────────────────────────────
CREATE INDEX IDX_NAVIRE_STATUT    ON NAVIRE(STATUT);
CREATE INDEX IDX_NAVIRE_NOM       ON NAVIRE(NOM);
CREATE INDEX IDX_QUAI_ETAT        ON QUAI(ETAT);
CREATE INDEX IDX_CAPTURE_NAVIRE   ON CAPTURE(ID_NAVIRE);
CREATE INDEX IDX_CAPTURE_DATE     ON CAPTURE(DATE_CAPTURE);
CREATE INDEX IDX_CAPTURE_STATUT   ON CAPTURE(STATUT);
CREATE INDEX IDX_AFFECTATION_QUAI ON AFFECTATION(QUAI_ID);

-- ── DONNÉES DE TEST ──────────────────────────────────────────

INSERT INTO QUAI VALUES (SEQ_QUAI.NEXTVAL,'Quai Alpha',15,3,'Zone Nord','Disponible',12.5,'Grue,Electricite,Eau',SYSDATE,NULL);
INSERT INTO QUAI VALUES (SEQ_QUAI.NEXTVAL,'Quai Beta', 20,8,'Zone Est', 'Disponible',15.0,'Grue,Carburant',      SYSDATE,NULL);
INSERT INTO QUAI VALUES (SEQ_QUAI.NEXTVAL,'Quai Gamma',10,10,'Zone Sud','Complet',    8.0,'Electricite,Eau',     SYSDATE,NULL);

INSERT INTO NAVIRE VALUES (SEQ_NAVIRE.NEXTVAL,'Neptune',  'TN-001','Chalutier', 250,'en mer');
INSERT INTO NAVIRE VALUES (SEQ_NAVIRE.NEXTVAL,'Poseidon', 'TN-002','Cargo',    5000,'a quai');
INSERT INTO NAVIRE VALUES (SEQ_NAVIRE.NEXTVAL,'Triton',   'TN-003','Palangrier',180,'en mer');
INSERT INTO NAVIRE VALUES (SEQ_NAVIRE.NEXTVAL,'Atlantis', 'TN-004','Chalutier', 320,'a quai');
INSERT INTO NAVIRE VALUES (SEQ_NAVIRE.NEXTVAL,'Daniel',   'TN-005','Artisanal', 100,'a quai');

INSERT INTO EMPLOYE VALUES (SEQ_EMPLOYE.NEXTVAL,'Ahmed Ben Ali', 'Capitaine de port','ahmed@port.tn', '+216 20 123 456',3500.00);
INSERT INTO EMPLOYE VALUES (SEQ_EMPLOYE.NEXTVAL,'Fatma Trabelsi','Agent maritime',   'fatma@port.tn', '+216 22 234 567',2800.00);
INSERT INTO EMPLOYE VALUES (SEQ_EMPLOYE.NEXTVAL,'Mohamed Gharbi','Technicien',       'gharbi@port.tn','+216 24 345 678',2200.00);

INSERT INTO CAPTURE VALUES (SEQ_CAPTURE.NEXTVAL,5,SYSDATE,   'Thon',   150,'Ahmed Ben Ali', 'Enregistre');
INSERT INTO CAPTURE VALUES (SEQ_CAPTURE.NEXTVAL,1,SYSDATE-1, 'Sardine',820,'Fatma Trabelsi','Valide');
INSERT INTO CAPTURE VALUES (SEQ_CAPTURE.NEXTVAL,3,SYSDATE-2, 'Merou',  125,'Ahmed Ben Ali', 'Livre');

INSERT INTO AFFECTATION VALUES (SEQ_AFFECTATION.NEXTVAL,'Poseidon',14.5,2,2,SYSDATE,  48,8.5,'Confirmee');
INSERT INTO AFFECTATION VALUES (SEQ_AFFECTATION.NEXTVAL,'Atlantis', 7.2,1,1,SYSDATE+1,24,7.8,'En attente');

COMMIT;

-- ── VÉRIFICATION ─────────────────────────────────────────────
SELECT 'QUAI'         AS TABLE_NAME, COUNT(*) AS NB FROM QUAI         UNION ALL
SELECT 'NAVIRE',       COUNT(*) FROM NAVIRE        UNION ALL
SELECT 'NAVIRE_ARCHIVE',COUNT(*) FROM NAVIRE_ARCHIVE UNION ALL
SELECT 'EMPLOYE',      COUNT(*) FROM EMPLOYE       UNION ALL
SELECT 'CAPTURE',      COUNT(*) FROM CAPTURE       UNION ALL
SELECT 'AFFECTATION',  COUNT(*) FROM AFFECTATION;
