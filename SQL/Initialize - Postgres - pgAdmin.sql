DO $$
    DECLARE _CustomerGUID UUID;
    DECLARE _CountOfHubWorldMaps INT;
    DECLARE _DefaultCharacterValuesID INT;
    DECLARE _CountOfDefaultCharacterValues INT;
BEGIN

    _CustomerGUID := '00000001-0001-0001-0001-000000000001';

    _CountOfHubWorldMaps := 0;
    _DefaultCharacterValuesID := 0;
    _CountOfDefaultCharacterValues := 0;

    SELECT CONCAT(COUNT(*), 0)
    FROM Maps
    WHERE CustomerGUID = _CustomerGUID
        AND ZoneName = 'HubWorld'
    INTO _CountOfHubWorldMaps;

    IF _CountOfHubWorldMaps < 1 THEN
        INSERT INTO Maps (CustomerGUID, MapName, MapData, Width, Height, ZoneName, WorldCompContainsFilter, WorldCompListFilter, MapMode, SoftPlayerCap, HardPlayerCap, MinutesToShutdownAfterEmpty)
        VALUES (_CustomerGUID, 'HubWorldMap', NULL, 1, 1, 'HubWorld', '', '', 1, 60, 80, 1);
    END IF;
	
	SELECT CONCAT(COUNT(*), 0)
    FROM Maps
    WHERE CustomerGUID = _CustomerGUID
        AND ZoneName = 'SouthGate'
    INTO _CountOfHubWorldMaps;

    IF _CountOfHubWorldMaps < 1 THEN
        INSERT INTO Maps (CustomerGUID, MapName, MapData, Width, Height, ZoneName, WorldCompContainsFilter, WorldCompListFilter, MapMode, SoftPlayerCap, HardPlayerCap, MinutesToShutdownAfterEmpty)
        VALUES (_CustomerGUID, 'HubWorldMap', NULL, 1, 1, 'SouthGate', '', '', 1, 60, 80, 1);
    END IF;

    SELECT CONCAT(COUNT(*), 0)
    FROM DefaultCharacterValues
    WHERE CustomerGUID = _CustomerGUID
        AND DefaultSetName = 'Default'
    INTO _CountOfDefaultCharacterValues;

    IF _CountOfDefaultCharacterValues < 1 THEN

        INSERT INTO DefaultCharacterValues (CustomerGUID, DefaultSetName, StartingMapName, X, Y, Z, RX, RY, RZ)
        VALUES (_CustomerGUID, 'Default', 'HubWorld', 1510, -160, 100, 0, 0, 0);

        _DefaultCharacterValuesID := CURRVAL(PG_GET_SERIAL_SEQUENCE('defaultcharactervalues', 'defaultcharactervaluesid'));

        INSERT INTO DefaultCustomCharacterData (CustomerGUID, DefaultCharacterValuesID, CustomFieldName, FieldValue)
        VALUES (_CustomerGUID, _DefaultCharacterValuesID, 'BaseCharacterStats', '{"Strength": 10, "Agility": 10, "Constitution": 10 }');
        INSERT INTO DefaultCustomCharacterData (CustomerGUID, DefaultCharacterValuesID, CustomFieldName, FieldValue)
        VALUES (_CustomerGUID, _DefaultCharacterValuesID, 'BaseCharacterSkills', '{ "Skill1": 1 }');
        INSERT INTO DefaultCustomCharacterData (CustomerGUID, DefaultCharacterValuesID, CustomFieldName, FieldValue)
        VALUES (_CustomerGUID, _DefaultCharacterValuesID, 'SupplyPodsOpened', '{}');
        INSERT INTO DefaultCustomCharacterData (CustomerGUID, DefaultCharacterValuesID, CustomFieldName, FieldValue)
        VALUES (_CustomerGUID, _DefaultCharacterValuesID, 'BagInventory', '{   "items":  }');
    END IF;
END $$;