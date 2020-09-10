void find_uinque_pairs_II()
{
  // This function finds unique pairs of employee tags which breaches social distancing norms
  // Tag - > Gateway Packet Structure
  // Self Id (2 Bytes) + Tags in proximity (10 Bytes = 5 * 2 Bytes) + Battery Status = 13 Bytes
  
  uint8_t a = 0; // No. of valid tags in a packet
  uint8_t ID1_LB, ID1_HB, ID2_LB, ID2_HB;
  uint8_t unique_flag = 1;
  uint8_t p,q,r,s;
  uint8_t add_entry = 1;
  uint8_t update_entry = 0;

//  if ((radioPktBuffer[5] == tempPktBuffer[5]) && (radioPktBuffer[6] == tempPktBuffer[6]) && (radioPktBuffer[7] == tempPktBuffer[7]))
//    add_entry = 0;

  for (uint8_t i = 0; i < event_count; i++)
  {
    if ((radioPktBuffer[1] == event_log[i*12]) && (radioPktBuffer[2] == event_log[i*12+1]) && (radioPktBuffer[3] == event_log[i*12+2]) && (radioPktBuffer[4] == event_log[i*12+3]) && (radioPktBuffer[5] == event_log[i*12+4]) && (radioPktBuffer[6] == event_log[i*12+5]) && (radioPktBuffer[7] == event_log[i*12+6]))
    {
      add_entry = 0;
      
      if (radioPktBuffer[10] != 0)
      {
        event_log[i*12+9] = radioPktBuffer[10]; 
      }
      break;
    }
  }
  
  // To avoid any error packets
  if (radioPktBuffer[0] == 12)
  {
    memcpy(tempPktBuffer, radioPktBuffer, sizeof(tempPktBuffer));
  
    if (add_entry == 1 )
    {
      // Determine whether it's a health packet or event packet
      for (uint8_t i = 0; i < 12; i++)
      {
        event_log[event_count * 12 + i] = radioPktBuffer[i+1];
      }
      event_count++;
    }
  }
}
