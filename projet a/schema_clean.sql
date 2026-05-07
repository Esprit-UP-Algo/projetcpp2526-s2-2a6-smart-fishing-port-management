-- ============================================================================
-- SCRIPT SQL COMPLET - BASE DE DONNÉES PORT MARITIME
-- Structures exactes selon la base Oracle existante
-- ============================================================================

-- ── 1. SUPPRESSION (ordre inverse des dépendances) ──────────────────────────

DROP TABLE AFFECTATION  CASCADE CONSTRAINTS PURGE;
DROP TABLE CAPTURE      CASCADE CONSTRAINTS PURGE;
DROP TABLE EMPLOYEE     CASCADE CONSTRAINTS PURGE;
DROP TABLE NAVIRE       CASCADE CONSTRAINTS PURGE;
DROP TABLE QUAI         CASCADE CONSTRAINTS PURGE;

-- ── 2. TABLE QUAI ───────────────────────────────────────────────────────────

CREATE TABLE QUAI (
    ID_QUAI          NUMBER        NOT NULL,
    NOM              VARCHAR2(100) NOT NULL,
    PLACES_TOTAL     NUMBER        NOT NULL,
    NAVIRES_PRESENTS NUMBER        DEFAULT 0,
    ADRESSE          VARCHAR2(200),
    ETAT             VARCHAR2(50),
    TIRANT_EAU_MAX   NUMBER(10,2),
    EQUIPEMENTS      VARCHAR2(500),
    DATE_DEBUT       DATE,
    DATE_FIN         DATE,
    CONSTRAINT PK_QUAI PRIMARY KEY (ID_QUAI)
);

CREATE SEQUENCE SEQ_QUAI START WITH 1 INCREMENT BY 1 NOCACHE;

-- ── 3. TABLE NAVIRE ─────────────────────────────────────────────────────────

CREATE TABLE NAVIRE (
    ID_NAVIRE        NUMBER        NOT NULL,
    NOM              VARCHAR2(100) NOT NULL,
    IMMATRICULATION  VARCHAR2(100),
    TYPE             VARCHAR2(100),
    CAPACITE         NUMBER,
    STATUT           VARCHAR2(50)  DEFAULT 'à quai',
    CONSTRAINT PK_NAVIRE PRIMARY KEY (ID_NAVIRE)
);

CREATE SEQUENCE SEQ_NAVIRE START WITH 1 INCREMENT BY 1 NOCACHE;

-- ── 4. TABLE EMPLOYEE ──────────────────────────────────────────────────────

CREATE TABLE EMPLOYEE (
    id               NUMBER        NOT NULL,
    nom              VARCHAR2(100) NOT NULL,
    poste            VARCHAR2(100),
    email            VARCHAR2(150),
    password         VARCHAR2(100),
    telephone        VARCHAR2(20),
    salaire          NUMBER(10,2),
    CONSTRAINT PK_EMPLOYEE PRIMARY KEY (id)
);

-- ── 5. TABLE CAPTURE ────────────────────────────────────────────────────────

CREATE TABLE CAPTURE (
    captureId        VARCHAR2(50)  NOT NULL,
    shipName         VARCHAR2(100),
    fishType         VARCHAR2(100),
    quantity         NUMBER,
    captureDate      DATE          DEFAULT SYSDATE,
    agent            VARCHAR2(100),
    CONSTRAINT PK_CAPTURE PRIMARY KEY (captureId)
);

CREATE SEQUENCE SEQ_CAPTURE START WITH 1 INCREMENT BY 1 NOCACHE;

-- ── 6. TABLE AFFECTATION ────────────────────────────────────────────────────

CREATE TABLE AFFECTATION (
    ID               NUMBER        NOT NULL,
    NAVIRE_NOM       VARCHAR2(100),
    TIRANT_NAVIRE    NUMBER(5,2),
    PRIORITE         NUMBER        DEFAULT 1,
    QUAI_ID          NUMBER,
    DATE_ARRIVEE     DATE,
    DUREE_ESTIMEE    NUMBER,
    SCORE_FINAL      NUMBER(5,2),
    STATUT           VARCHAR2(50)  DEFAULT 'En attente',
    CONSTRAINT PK_AFFECTATION PRIMARY KEY (ID),
    CONSTRAINT FK_AFFECTATION_QUAI FOREIGN KEY (QUAI_ID) REFERENCES QUAI(ID_QUAI) ON DELETE SET NULL
);

CREATE SEQUENCE SEQ_AFFECTATION START WITH 1 INCREMENT BY 1 NOCACHE;

-- ── 7. INDEX ────────────────────────────────────────────────────────────────

CREATE INDEX IDX_NAVIRE_STATUT     ON NAVIRE(STATUT);
CREATE INDEX IDX_QUAI_ETAT         ON QUAI(ETAT);
CREATE INDEX IDX_CAPTURE_NAVIRE    ON CAPTURE(ID_NAVIRE);
CREATE INDEX IDX_CAPTURE_DATE      ON CAPTURE(DATE_CAPTURE);
CREATE INDEX IDX_AFFECTATION_QUAI  ON AFFECTATION(QUAI_ID);
CREATE INDEX IDX_EMPLOYEE_EMAIL     ON EMPLOYEE(email);

-- ── 8. DONNÉES DE TEST ──────────────────────────────────────────────────────

INSERT INTO QUAI VALUES (SEQ_QUAI.NEXTVAL,'Quai Alpha',15,3,'Zone Nord','Disponible',12.5,'Grue,Électricité,Eau',SYSDATE,NULL);
INSERT INTO QUAI VALUES (SEQ_QUAI.NEXTVAL,'Quai Beta', 20,8,'Zone Est', 'Disponible',15.0,'Grue,Carburant',      SYSDATE,NULL);
INSERT INTO QUAI VALUES (SEQ_QUAI.NEXTVAL,'Quai Gamma',10,10,'Zone Sud','Complet',    8.0,'Électricité,Eau',     SYSDATE,NULL);

INSERT INTO NAVIRE VALUES (SEQ_NAVIRE.NEXTVAL,'Neptune',  'TN-001','Chalutier', 250,'en mer');
INSERT INTO NAVIRE VALUES (SEQ_NAVIRE.NEXTVAL,'Poséidon', 'TN-002','Cargo',    5000,'à quai');
INSERT INTO NAVIRE VALUES (SEQ_NAVIRE.NEXTVAL,'Triton',   'TN-003','Palangrier',180,'en mer');
INSERT INTO NAVIRE VALUES (SEQ_NAVIRE.NEXTVAL,'Atlantis', 'TN-004','Chalutier', 320,'à quai');

INSERT INTO EMPLOYEE (id,nom,poste,email,password,telephone,salaire) VALUES (1,'Ahmed Ben Ali','Superviseur','ahmed@port.tn','ahmed123','+216 20 123 456',3500.00);
INSERT INTO EMPLOYEE (id,nom,poste,email,password,telephone,salaire) VALUES (2,'Fatma Trabelsi','Chef de quai',  'fatma@port.tn', 'fatma123','+216 22 234 567',2800.00);
INSERT INTO EMPLOYEE (id,nom,poste,email,password,telephone,salaire) VALUES (3,'Mohamed Gharbi','Docker',      'gharbi@port.tn','gharbi123','+216 24 345 678',2200.00);
INSERT INTO EMPLOYEE (id,nom,poste,email,password,telephone,salaire) VALUES (4,'Ali Mansour','Pêcheur',    'ali@port.tn','ali123','+216 26 456 789',2000.00);
INSERT INTO EMPLOYEE (id,nom,poste,email,password,telephone,salaire) VALUES (5,'Sami Khelifi','Matelot',    'sami@port.tn','sami123','+216 28 567 890',2100.00);

INSERT INTO CAPTURE VALUES ('CAP-001','Neptune',  'Thon',   450, SYSDATE-5,'Ahmed Ben Ali');
INSERT INTO CAPTURE VALUES ('CAP-002','Triton',   'Sardine',820, SYSDATE-3,'Fatma Trabelsi');
INSERT INTO CAPTURE VALUES ('CAP-003','Neptune',  'Mérou',  125, SYSDATE-1,'Ahmed Ben Ali');

INSERT INTO AFFECTATION VALUES (SEQ_AFFECTATION.NEXTVAL,'Poséidon',14.5,2,2,SYSDATE,48,8.5,'Confirmee');
INSERT INTO AFFECTATION VALUES (SEQ_AFFECTATION.NEXTVAL,'Atlantis', 7.2,1,1,SYSDATE+1,24,7.8,'En attente');

COMMIT;

-- ── 9. VÉRIFICATION ─────────────────────────────────────────────────────────

SELECT 'QUAI'        AS TABLE_NAME, COUNT(*) AS NB FROM QUAI        UNION ALL
SELECT 'NAVIRE',      COUNT(*) FROM NAVIRE       UNION ALL
SELECT 'EMPLOYEE',    COUNT(*) FROM EMPLOYEE     UNION ALL
SELECT 'CAPTURE',     COUNT(*) FROM CAPTURE      UNION ALL
SELECT 'AFFECTATION', COUNT(*) FROM AFFECTATION;
