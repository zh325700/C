#define _GNU_SOURCE 
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "time.h"
#include <sqlite3.h>
#include <string.h>
#include "sensor_db.h"
#include <inttypes.h>


   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char sql[300];

//int callback_t(void *, int, char **, char **); //在主函数写具体的内容


DBCONN * init_connection(char clear_up_flag)       //clear up flag 是干什么的？
{

    rc = sqlite3_open(TO_STRING(DB_NAME), &db);              // have to use TO_STRING t change define DB_NAME to STRING otherwise it will not work.
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }
    
    if(clear_up_flag)                          //if flag==1 then drop the formeer one , and create a new one 
    {
snprintf(sql,200,"DROP TABLE IF EXISTS %s;CREATE TABLE %s(Id INTEGER PRIMARY KEY,sensor_id INT, sensor_value DECIMAL(4,2), timestamp TIMESTAMP);", TO_STRING(TABLE_NAME), TO_STRING(TABLE_NAME));
        }
    else                                                        // asprintf rather than sprintf . avoid exceed the buf size
    {
        snprintf(sql,200,"CREATE TABLE IF NOT EXISTS %s(Id INTEGER PRIMARY KEY AUTOINCREMENT,sensor_id INT, sensor_value DECIMAL(4,2), timestamp TIMESTAMP);", TO_STRING(TABLE_NAME));
        }
    
    
    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK ) {
        
    fprintf(stderr, "Failed to create table\n");
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(db);
        
    } else {
        
        fprintf(stdout, "Table created successfully\n");
    }
   
    return db;

}
/*
 * Disconnect from the database server
 */
void disconnect(DBCONN *conn)
{
sqlite3_close(conn);
    }


/*
 * Write an INSERT query to insert a single sensor measurement
 * Return zero for success, and non-zero if an error occurs
 */
int insert_sensor(DBCONN * conn, sensor_id_t id, sensor_value_t value, sensor_ts_t ts)
{

    

    snprintf(sql,200, "INSERT INTO %s (sensor_id,sensor_value,timestamp) VALUES(%"PRIu16", %f, %ld);" , TO_STRING(TABLE_NAME), id, value, ts);
    rc = sqlite3_exec(conn, sql, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK ) {
        
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(conn);
    return -1;
        
    } 
    return 0;

}


/*
 * Write an INSERT query to insert all sensor measurements available in the file 'sensor_data'
 * Return zero for success, and non-zero if an error occurs
 */
int insert_sensor_from_file(DBCONN * conn, FILE * sensor_data)
{

    sensor_data_t *data;
    data = (sensor_data_t *) malloc(sizeof(sensor_data_t));
    int result;
    while((fread(&data->id, sizeof(sensor_id_t), 1, sensor_data)==1)
        &&(fread(&data->value, sizeof(sensor_value_t), 1, sensor_data)==1)
        &&(fread(&data->ts, sizeof(sensor_ts_t), 1, sensor_data)==1))
        {
            result = insert_sensor(conn,data->id,data->value,data->ts);
            if(result!=0)
            {
                return result;                //if read failed return non zero
                }
            }
    free(data);

    return result;
}


/*
  * Write a SELECT query to select all sensor measurements in the table 
  * The callback function is applied to every row in the result
  * Return zero for success, and non-zero if an error occurs
  */
int find_sensor_all(DBCONN * conn, callback_t f)
{


    
    snprintf(sql,200, "SELECT * FROM %s" , TO_STRING(TABLE_NAME));
    rc = sqlite3_exec(conn, sql, f, 0, &zErrMsg);               //here call the callback function
    if (rc != SQLITE_OK ) {
        
    fprintf(stderr, "Failed to select the sensor data from table\n");
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(conn);
    return -1;
        
    } else {
        
        fprintf(stdout, "select successfully\n");
        
    }
    
    return 0;
}


/*
 * Write a SELECT query to return all sensor measurements having a temperature of 'value'
 * The callback function is applied to every row in the result
 * Return zero for success, and non-zero if an error occurs
 */
int find_sensor_by_value(DBCONN * conn, sensor_value_t value, callback_t f)
{


    snprintf(sql,200, "SELECT * FROM %s WHERE sensor_value = %f" , TO_STRING(TABLE_NAME),value);
    rc = sqlite3_exec(conn, sql, f, 0, &zErrMsg);               //here call the callback function
    if (rc != SQLITE_OK ) {
        
    fprintf(stderr, "Failed to select the sensor data from table\n");
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(conn);
    return -1;
        
    } else {
        
        fprintf(stdout, "select successfully\n");
        
    }
    
    return 0;
}


/*
 * Write a SELECT query to return all sensor measurements of which the temperature exceeds 'value'
 * The callback function is applied to every row in the result
 * Return zero for success, and non-zero if an error occurs
 */
int find_sensor_exceed_value(DBCONN * conn, sensor_value_t value, callback_t f)
{

    snprintf(sql,200, "SELECT * FROM %s WHERE sensor_value > %f" , TO_STRING(TABLE_NAME),value);
    rc = sqlite3_exec(conn, sql, f, 0, &zErrMsg);               //here call the callback function
    if (rc != SQLITE_OK ) {
        
    fprintf(stderr, "Failed to select the sensor data from table\n");
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(conn);
    return -1;
        
    } else {
        
        fprintf(stdout, "select successfully\n");
        
    }
    
    return 0;    
}


/*
 * Write a SELECT query to return all sensor measurements having a timestamp 'ts'
 * The callback function is applied to every row in the result
 * Return zero for success, and non-zero if an error occurs
 */
int find_sensor_by_timestamp(DBCONN * conn, sensor_ts_t ts, callback_t f)
{


    snprintf(sql,200, "SELECT * FROM %s WHERE timestamp =%ld" , TO_STRING(TABLE_NAME),ts);
    rc = sqlite3_exec(conn, sql, f, 0, &zErrMsg);               //here call the callback function
    if (rc != SQLITE_OK ) {
        
    fprintf(stderr, "Failed to select the sensor data from table by timestanp \n");
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(conn);
    return -1;
        
    } else {
        
        fprintf(stdout, "select successfully by timestanp\n");
        
    }

    return 0;
}


/*
 * Write a SELECT query to return all sensor measurements recorded after timestamp 'ts'
 * The callback function is applied to every row in the result
 * return zero for success, and non-zero if an error occurs
 */
int find_sensor_after_timestamp(DBCONN * conn, sensor_ts_t ts, callback_t f)
{

    
    snprintf(sql,200, "SELECT * FROM %s WHERE timestamp >%ld" , TO_STRING(TABLE_NAME),ts);                    // bigger timestamp means later date is generated
    rc = sqlite3_exec(conn, sql, f, 0, &zErrMsg);               //here call the callback function
    if (rc != SQLITE_OK ) {
        
    fprintf(stderr, "Failed to select the sensor data from table after timestanp %ld\n",ts);
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    sqlite3_close(conn);
    return -1;
        
    } else {
        
        fprintf(stdout, "select successfully after timestanp %ld\n",ts);
        
    }
   
    return 0;    
}