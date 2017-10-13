<?xml version="1.0" encoding="UTF-8"?>
<app:application xmlns:app="http://www.sierrawireless.com/airvantage/application/1.0"
        type="linkwave.sensorTest"
        name="lcdTemperature"
        revision="0.0.4">
 
    <capabilities>
        <communication>
            <protocol comm-id="IMEI" type="MQTT" />
        </communication>
    
        <data>
            <encoding type="MQTT">
                <asset default-label="Sensor" id="sensor">
                    <variable default-label="Temperature0" path="temperature0" type="double"/>
                    <variable default-label="Temperature1" path="temperature1" type="double"/>
                    <variable default-label="Temperature2" path="temperature2" type="double"/>
                    <variable default-label="Temperature3" path="temperature3" type="double"/>
                </asset>
            </encoding>
        </data> 
 
    </capabilities>
</app:application>
