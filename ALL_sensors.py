import paho.mqtt.client as mqtt
import mysql.connector
import json

# MQTT configuration
MQTT_BROKER_HOST = "broker.emqx.io"
MQTT_BROKER_PORT = 1883
MQTT_TOPIC = "Sensor"

# MySQL configuration
MYSQL_HOST = "localhost"
MYSQL_USER = "root"
MYSQL_PASSWORD = ""
MYSQL_DATABASE = "DHT_data"

# MQTT callback when a message is received
def on_message(client, userdata, message):
    payload = message.payload.decode("utf-8")
    insert_data(payload)

# Function to create the database if it doesn't exist
def create_database_if_not_exists():
    try:
        # Connect to MySQL server without specifying the database
        connection = mysql.connector.connect(
            host=MYSQL_HOST,
            user=MYSQL_USER,
            password=MYSQL_PASSWORD
        )

        # Create a cursor object to execute SQL queries
        cursor = connection.cursor()

        # Define the SQL query to create the database if it doesn't exist
        create_database_query = f"CREATE DATABASE IF NOT EXISTS {MYSQL_DATABASE}"

        # Execute the SQL query to create the database
        cursor.execute(create_database_query)

        # Close the cursor and the database connection
        cursor.close()
        connection.close()

        print(f"Database '{MYSQL_DATABASE}' created or already exists")

    except Exception as e:
        print(f"Error: {str(e)}")

# Create the database if it doesn't exist
create_database_if_not_exists()

def create_table_if_not_exists():
    try:
        # Connect to the MySQL database
        connection = mysql.connector.connect(
            host=MYSQL_HOST,
            user=MYSQL_USER,
            password=MYSQL_PASSWORD,
            database=MYSQL_DATABASE
        )

        # Create a cursor object to execute SQL queries
        cursor = connection.cursor()

        # Define the SQL query to create the table if it doesn't exist
        create_table_query = """
        CREATE TABLE IF NOT EXISTS sensor_data (
            id INT AUTO_INCREMENT PRIMARY KEY,
            temperature FLOAT,
            humidity FLOAT,
            IR_sensor VARCHAR(255),
            LDRvalue INT,
            Gasvalue FLOAT,
            timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
        """

        # Print the query to check if it's correct
        print("Creating table with query:")
        print(create_table_query)

        # Execute the SQL query to create the table
        cursor.execute(create_table_query)

        # Commit the transaction
        connection.commit()

        print("Table created successfully")

        # Close the cursor and the database connection
        cursor.close()
        connection.close()
    except Exception as e:
        print(f"Error: {str(e)}")

# Create the table if it doesn't exist
create_table_if_not_exists()

# Function to insert data into MySQL
def insert_data(data):
    try:
        # Split the received data by comma to extract individual values
        values = data.split(',')

        # Check if there are enough values
        if len(values) == 5:
            # Extract values
            temperature = float(values[0])
            humidity = float(values[1])
            IR_sensor = values[2]
            LDRvalue = int(values[3])
            Gasvalue = float(values[4])
            

            # Create a dictionary to store the data
            data_dict = {
                "temperature": temperature,
                "humidity": humidity,
                "IR_sensor": IR_sensor,
                "LDRvalue": LDRvalue,
                "Gasvalue": Gasvalue
            }

            # Convert the dictionary to a JSON string
            json_data = json.dumps(data_dict)

            # Connect to the MySQL database
            connection = mysql.connector.connect(
                host=MYSQL_HOST,
                user=MYSQL_USER,
                password=MYSQL_PASSWORD,
                database=MYSQL_DATABASE
            )

            # Create a cursor object to execute SQL queries
            cursor = connection.cursor()

            # Define the SQL query to insert data into the database
            sql_insert_query = "INSERT INTO sensor_data (temperature, humidity, IR_sensor, LDRvalue, Gasvalue) VALUES (%s, %s, %s, %s, %s)"

            # Execute the SQL query with data parameters
            cursor.execute(sql_insert_query, (temperature, humidity, IR_sensor, LDRvalue, Gasvalue))

            # Commit the transaction
            connection.commit()

            print(f"Data inserted: temperature={temperature}, humidity={humidity}, IR_sensor={IR_sensor}, LDRvalue={LDRvalue}, Gasvalue={Gasvalue}")

            # Close the cursor and the database connection
            cursor.close()
            connection.close()
        else:
            print("Invalid data format: Expected 5 values separated by commas")
    except Exception as e:
        print(f"Error: {str(e)}")

# MQTT client setup
client = mqtt.Client()
client.on_message = on_message

# Connect to MQTT broker
client.connect(MQTT_BROKER_HOST, MQTT_BROKER_PORT, 60)
client.subscribe(MQTT_TOPIC)

# Start the MQTT loop
client.loop_forever()
