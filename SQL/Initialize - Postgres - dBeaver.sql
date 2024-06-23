@set CustomerGUID = '<Your Hub World MMO API Key>'
	
insert into Maps (CustomerGUID, MapName, MapData, Width, Height, ZoneName, WorldCompContainsFilter, WorldCompListFilter, MapMode, SoftPlayerCap, HardPlayerCap, MinutesToShutdownAfterEmpty)
select :CustomerGUID, 'HubWorldMap', NULL, 1, 1, 'HubWorld', '', '', 1, 60, 80, 1
where not exists (select 1 from Maps M where M.CustomerGUID = :CustomerGUID and M.ZoneName = 'HubWorld');

insert into Maps (CustomerGUID, MapName, MapData, Width, Height, ZoneName, WorldCompContainsFilter, WorldCompListFilter, MapMode, SoftPlayerCap, HardPlayerCap, MinutesToShutdownAfterEmpty)
select :CustomerGUID, 'HubWorldMap', NULL, 1, 1, 'SouthGate', '', '', 1, 60, 80, 1
where not exists (select 1 from Maps M where M.CustomerGUID = :CustomerGUID and M.ZoneName = 'SouthGate');

with dcvInsert as (
    insert into DefaultCharacterValues (CustomerGUID, DefaultSetName, StartingMapName, X, Y, Z, RX, RY, RZ)
    select :CustomerGUID, 'Default', 'HubWorld', 1510, -160, 100, 0, 0, 0
    where not exists (select 1 from DefaultCharacterValues DCV where DCV.CustomerGUID = :CustomerGUID and DCV.DefaultSetName = 'Default')
    returning *
)
insert into DefaultCustomCharacterData (CustomerGUID, DefaultCharacterValuesID, CustomFieldName, FieldValue)
values 
	((select CustomerGUID from dcvInsert), (select DefaultCharacterValuesId from dcvInsert), 'BaseCharacterStats', '{"Strength": 10, "Agility": 10, "Constitution": 10 }' ),
	((select CustomerGUID from dcvInsert), (select DefaultCharacterValuesId from dcvInsert), 'BaseCharacterSkills', '{ "Skill1": 1 }'),
	((select CustomerGUID from dcvInsert), (select DefaultCharacterValuesId from dcvInsert), 'SupplyPodsOpened', '{}'),
	((select CustomerGUID from dcvInsert), (select DefaultCharacterValuesId from dcvInsert), 'BagInventory', '{   "items":  }')
;

