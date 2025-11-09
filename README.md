# RFID-Attendance-System
Semester end project:It automates attendance tracking using **RFID cards**, an **ESP8266 microcontroller**, and **Google Sheets integration**.  
## Team Members
- Mrunal Patankar  
- Divya Gaikwad 
- Diya Anthony  
## Features
- Detects student RFID cards in real-time.
- Marks attendance automatically in **Google Sheets** with **timestamp**.
- Displays attendance on a **web dashboard** hosted by ESP8266.
- Calculates **percentage attendance** dynamically.
- Works without modifying the Google Sheet manually.
- Future-ready for **Looker Studio integration** for reporting and analytics.
## Technology Stack
- **Hardware:** ESP8266, RFID Reader (MFRC522), PIR Sensor, LEDs, Buzzer  
- **Software:** Arduino IDE, C++  
- **Cloud Services:** Google Sheets API, Google Apps Script  
- **Networking:** Wi-Fi (ESP8266), HTTP requests  
## Project Flow
1. ESP8266 detects motion using PIR sensor.
2. RFID card scan triggers attendance logging.
3. Timestamped attendance is sent to Google Sheets via **Apps Script API**.
4. Web dashboard shows real-time attendance status.
5. Attendance percentages are calculated automatically.
## How to Use
1. Scan the RFID card near the sensor.
2. Attendance is marked automatically in Google Sheets.
3. Visit the ESP8266 dashboard to see real-time attendance updates.
## Folder Structure
google gs code.txt # Google Apps Script code
├google-sheet_final_attendance.ino# Arduino IDE code
IEEE final report #report

## Notes
- The system can be scaled for multiple classrooms.
- Looker Studio can be connected for advanced reporting.
- No manual updates required in Google Sheets; everything is automated.

