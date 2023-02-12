DECLARE @CustomerGUID uniqueidentifier = ''

DECLARE @CountOfHubWorldMaps int = 0

SELECT @CountOfHubWorldMaps=COUNT(*) FROM Maps WHERE CustomerGUID=@CustomerGUID AND MapName='HubWorld'

IF (@CountOfHubWorldMaps < 1)
BEGIN

INSERT INTO Maps ([CustomerGUID], [MapName], [MapData], [Width], [Height], [ZoneName], [WorldCompContainsFilter], [WorldCompListFilter], [MapMode], [SoftPlayerCap], [HardPlayerCap], [MinutesToShutdownAfterEmpty])
VALUES (@CustomerGUID, 'HubWorld', NULL, 1, 1, 'HubWorld', '', '', 1, 60, 80, 1)

END