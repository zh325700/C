

#include "datamgr.h"
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "lib/dplist.h"
#include "time.h"
int main(void)
{
FILE *fp_sensor_map;
FILE * fp_sensor_data;
fp_sensor_map=fopen ("room_sensor.map", "r");
fp_sensor_data=fopen ("sensor_data", "r");
datamgr_parse_sensor_files(fp_sensor_map,fp_sensor_data);
printf("the avg of the room is : %f \n",datamgr_get_avg(21));
printf("the last modified of the room is : %ld \n",datamgr_get_last_modified(15));

return 0;
}
