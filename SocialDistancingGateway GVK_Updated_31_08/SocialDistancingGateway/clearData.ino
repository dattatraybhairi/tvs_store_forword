void clearData()
{
  memset(unique_pair_data, 0, sizeof(unique_pair_data));
  memset(event_log, 0, sizeof(event_log));
  memset(health_data, 0, sizeof(health_data));
  unique_count = 0;
  health_count = 0;
  event_count = 0;
}
