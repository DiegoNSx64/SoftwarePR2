/*
import paho.mqtt.client as mqtt
import psycopg
import json
from datetime import datetime

DB_PARAMS = {
    "dbname": "proyecto_pr2",
    "user": "postgres",
    "password": "postgres",
    "host": "localhost",
    "port": "5432"
}

CONFIG_CONTROL = {
    "esp1": {"co2_limite": 800, "o2_min": 19},
    "esp2": {"temp_max": 38.5, "distancia_min": 10.0},
    "esp3": {"vibe_max": 2.0}
}
#   Ejemplo mensaje recibido por el MQTT;
#       {
#       "temperatura": 37.2,
#       "vibracion": 0.15,
#       "o2": 20.5,
#       "co2": 5.1,
#       }
#
#       {
#       "parada_emergencia": 1
#       }
#
#       {
#       "placa_procesada": 1
#       }


MQTT_USER = "giirob"
MQTT_PASS = "UPV2024"
MQTT_BROKER = "mqtt.dsic.upv.es"
MQTT_PORT = 1883

def guardar_en_db(topic, payload):
    try:
        # Extraemos el ID del dispositivo (ej: "nombre_laboratorio/cod_planta/cod_incubadora")
        partes_topic = topic.split('/')
        cod_planta = int(partes_topic[1])
        cod_incubadora = int(partes_topic[2])
        datos = json.loads(payload)
       
        with psycopg.connect(**DB_PARAMS) as conn:
            with conn.cursor() as cur:

                for parametro, valor in datos.items():
   
                    # Guardar datos lectura
                    #cur.execute(
                        #"INSERT INTO incubadora.lecturas_sensores (dispositivo, tipo_parametro, valor) VALUES (%s, %s, %s)",
                        #(dispositivo, parametro, valor)
                    #)
                    if parametro == "parada_emergencia" and valor == 1:
                        cur.execute(
                            "UPDATE Planta SET n_paradas = n_paradas + 1 WHERE codigo = %s;"
                            (cod_planta,)
                        )
                    elif parametro == "placa_procesada" and valor == 1:
                        cur.execute(
                            "UPDATE Planta SET n_procesadas = n_procesadas + 1 WHERE codigo = %s;"
                            (cod_planta,)
                        )
                    else:


                        temp = datos.get("temperatura")
                        vibracion = datos.get("vibracion")
                        o2 = datos.get("o2")
                        co2 = datos.get("co2")

                        cur.execute(
                            "UPDATE Incubadora SET temperatura = %s, vibracion = %s, o2 = %s, co2 = %s WHERE codigo = %s; ", (temp, vibracion, o2, co2, cod_incubadora)
                        )
                        cur.execute(
                            "INSERT INTO medidas(codigo_planta, codigo_incubadora, temperatura_m, vibracion_m, o2_m, co2_m) VALUES (%s, %s, %s, %s, %s, %s);", (cod_planta, cod_incubadora, temp, vibracion, o2, co2)
                        )
                        break
               
                # Guardar cambios
                conn.commit()
                print(f"Datos procesados correctamente.")

    except Exception as e:
        print(f"Error procesando mensaje: {e}")

# --- 3. CONFIGURACIÓN MQTT ---

def on_message(client, userdata, msg):
    guardar_en_db(msg.topic, msg.payload)

client = mqtt.Client()
client.username_pw_set(MQTT_USER, MQTT_PASS)
client.on_message = on_message

client.connect(MQTT_BROKER, MQTT_PORT)
client.subscribe("incubadora/#")

print("Servidor de la incubadora escuchando MQTT...")
client.loop_forever()

*/
