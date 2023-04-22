DELIMITER //

SET @CustomerGUID = '';

SET @CountOfHubWorldMaps = 0;
SET @DefaultCharacterValuesID = 0;
SET @CountOfDefaultCharacterValues = 0;

SELECT CONCAT(COUNT(*), 0)
INTO @CountOfHubWorldMaps
FROM Maps
WHERE CustomerGUID = @CustomerGUID
    AND MapName = 'HubWorldMap';


IF @CountOfHubWorldMaps < 1 THEN
    INSERT INTO Maps (CustomerGUID, MapName, MapData, Width, Height, ZoneName, WorldCompContainsFilter, WorldCompListFilter, MapMode, SoftPlayerCap, HardPlayerCap, MinutesToShutdownAfterEmpty)
    VALUES (@CustomerGUID, 'HubWorldMap', NULL, 1, 1, 'HubWorld', '', '', 1, 60, 80, 1);
END IF;

SELECT CONCAT(COUNT(*), 0)
INTO @CountOfDefaultCharacterValues
FROM DefaultCharacterValues
WHERE CustomerGUID = @CustomerGUID
    AND DefaultSetName = 'Default';

IF @CountOfDefaultCharacterValues < 1 THEN

    INSERT INTO DefaultCharacterValues (CustomerGUID, DefaultSetName, StartingMapName, X, Y, Z, RX, RY, RZ)
    VALUES (@CustomerGUID, 'Default', 'HubWorld', 1510, -160, 100, 0, 0, 0);

    SET @DefaultCharacterValuesID = LAST_INSERT_ID();

    INSERT INTO DefaultCustomCharacterData (CustomerGUID, DefaultCharacterValuesID, CustomFieldName, FieldValue)
    VALUES (@CustomerGUID, @DefaultCharacterValuesID, 'BaseCharacterStats', '{"Strength": 10, "Agility": 10, "Constitution": 10 }');
    INSERT INTO DefaultCustomCharacterData (CustomerGUID, DefaultCharacterValuesID, CustomFieldName, FieldValue)
    VALUES (@CustomerGUID, @DefaultCharacterValuesID, 'BaseCharacterSkills', '{ "Skill1": 1 }');
    INSERT INTO DefaultCustomCharacterData (CustomerGUID, DefaultCharacterValuesID, CustomFieldName, FieldValue)
    VALUES (@CustomerGUID, @DefaultCharacterValuesID, 'SupplyPodsOpened', '{}');
    INSERT INTO DefaultCustomCharacterData (CustomerGUID, DefaultCharacterValuesID, CustomFieldName, FieldValue)
    VALUES (@CustomerGUID, @DefaultCharacterValuesID, 'BagInventory', '{   "items":  }');
END IF;

// DELIMITER ;
