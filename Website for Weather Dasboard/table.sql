CREATE TABLE SensorData (
    Temperature VARCHAR(10),
    Humidity VARCHAR(10),
    Pressure VARCHAR(10),
    Rainfall VARCHAR(10),
    Wind Speed VARCHAR(10),
    Wind Direction VARCHAR(10),
    Daily Rainfall VARCHAR(10),
    Time TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
)
