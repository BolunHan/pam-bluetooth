Intro
---

Ok this module is forked from `https://github.com/c4tz/pam-bluetooth/blob/main/Makefile`. All credit to the previous author and ChatGPT.

I put together the installation guide so that it can be used on ubuntu 24.04+ with ease.

Usage
-----
# Setting Up PAM Bluetooth Authentication on Ubuntu

This guide outlines the steps to install and configure the `pam_bluetooth` module for enabling Bluetooth-based authentication on your Ubuntu system.

---

## 1. Clone the Repository
Clone the `pam_bluetooth` source code from the GitHub repository:
```bash
git clone https://github.com/BolunHan/pam-bluetooth.git
cd pam-bluetooth
```

## 2. Install Dependencies
Install the necessary development package for PAM:
```bash
sudo apt update
sudo apt install libpam0g-dev
```

## 3. Compile the Module
Compile the `pam_bluetooth` source code:
```bash
gcc -fPIC -fno-stack-protector -c pam_bluetooth.c
```

## 4. Install the PAM Module
Create the required directory for PAM security modules if it does not already exist:
```bash
sudo mkdir -p /lib/security
```

Move the compiled module to the security directory:
```bash
sudo ld -x --shared -o /lib/security/pam_bluetooth.so pam_bluetooth.o
```

## 5. Configure Authorized Bluetooth Devices
Create a configuration file for storing authorized Bluetooth devices:
```bash
sudo touch /etc/security/authorized_bluetooth.conf
```

Add the MAC addresses of authorized Bluetooth devices to this file. Each MAC address should be on a new line. For example:
```plaintext
XX:XX:XX:XX:XX:XX
YY:YY:YY:YY:YY:YY
```

## 6. Integrate with PAM
Edit the PAM configuration file for authentication (e.g., `/etc/pam.d/gdm-password` for gdm lockscreen) to include the `pam_bluetooth` module. Add the following line at the beginning of the file:
```text
auth sufficient pam_bluetooth.so
```

## 7. Test the Configuration
Restart the system or switch users to test the new Bluetooth authentication setup. Ensure your Bluetooth device is paired and authorized.

---

### Troubleshooting
- Ensure that your Bluetooth device is within range and powered on.
- Check for errors in the system logs:
  ```bash
  sudo journalctl -xe
  ```

---

### Notes
- This setup assumes basic knowledge of Linux PAM and Bluetooth configurations.
- Use this method cautiously on production systems, as it may introduce security risks if not properly configured.

