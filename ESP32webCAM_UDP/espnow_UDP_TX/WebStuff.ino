
void handleCmd() 
{
  String out;

  for (uint8_t i = 0; i < server.args(); i++)
  {
    if      (server.argName(i) == "FK") 
      fakeRC = !fakeRC;
    else if (server.argName(i) == "SI") 
      sendInterval = server.arg(i).toInt();
    else if (server.argName(i) == "PO") 
      remoteUdpPort = server.arg(i).toInt();
    else if (server.argName(i) == "IP") 
      udpAddress = server.arg(i);
  }
  
  out += "<html><br><br><center>\n";

  out += "<form method=\"post\">\n";
  out += "Remote IP \n";
  out += "<input type=\"text\" name=\"IP\" value=\"";
  out += udpAddress;
  out += "\">&emsp;\n";       
  out += "Remote Port \n";
  out += "<input type=\"number\" name=\"PO\" style=\"width:6em\" value=\"";
  out += remoteUdpPort;
  out += "\">&emsp;\n";
  out += "<input type=\"submit\"><br>\n";
  out += "</form>\n";
  out += "<br>";

  out += "<form method=\"post\">\n";
  out += "TX Interval&emsp;\n";
  out += "<input type=\"number\" name=\"SI\" style=\"width:6em\" value=\"";
  out += sendInterval;
  out += "\">&emsp;\n";
  out += "<input type=\"submit\"><br>\n";
  out += "</form>\n";
  out += "<br>";

  if (fakeRC)
    out += "<button onclick=\"window.location.href='?FK\';\">Real RC</button>\n";
  else
    out += "<button onclick=\"window.location.href='?FK\';\">Make fake RC</button>\n";

  out += "<br>";
  out += "</center></html>";
  server.send(200, "text/html", out);    
}

void handleNotFound() 
{
  server.send(404, "text/plain", "Not Here");
}
