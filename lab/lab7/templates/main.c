#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "time.h"
#include <sqlite3.h>
#include <string.h>
#include "sensor_db.h"
int main()
{
FILE *fp;
fp=fopen("sensor_data", "r");
DBCONN *con;
con=init_connection(1);
insert_sensor_from_file(con,fp);
disconnect(con);
return 0;
}
