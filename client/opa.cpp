/*

static void dummy()
{

  ClientConfig config("localhost", 10000);
  Client client(config);
  msg_req_handshake msg_hs;
  msg_req_echo msg_echo;
  msg_req_shutdown msg_req_sd;
  msg_res_shutdown msg_res_sd;

  msg_hs.m_msg_id = MSG_REQ_ECHO;
  msg_echo.m_len = 13;
  client.Connect();
  client.Write(&msg_hs, sizeof(msg_hs));
  client.Write(&msg_echo, sizeof(msg_echo));
  client.Write((void *)"Ola, Mundo!\n", 13);
  client.Close();

  msg_hs.m_msg_id = MSG_REQ_ECHO;
  msg_echo.m_len = 11;
  client.Connect();
  client.Write(&msg_hs, sizeof(msg_hs));
  client.Write(&msg_echo, sizeof(msg_echo));
  client.Write((void *)"Tudo bem?\n", 11);
  client.Close();

  msg_hs.m_msg_id = MSG_REQ_SHUTDOWN;
  memset(&msg_req_sd, 0, sizeof(msg_req_sd));
  memcpy(&msg_req_sd.m_user, "root", 4);
  memcpy(&msg_req_sd.m_pass, "manager", 7);
  msg_req_sd.m_timeout = 5000;

  client.Connect();
  client.Write(&msg_hs, sizeof(msg_hs));
  client.Write(&msg_req_sd, sizeof(msg_req_sd));
  client.Read(&msg_res_sd, sizeof(msg_res_sd));
  client.Close();

  if (msg_res_sd.m_ok)
  {
    printf("Server vai desligar!\n");
  }
  else
  {
    printf("Server NAO vai desligar!\n");
  }
  
}
*/