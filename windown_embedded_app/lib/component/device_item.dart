import 'package:flutter/material.dart';

class DeviceItem extends StatelessWidget {
  final String portName;
  final String friendlyName;
  final String hardwareID;
  final String manufactureName;

  const DeviceItem({
    Key? key,
    required this.portName,
    required this.friendlyName,
    required this.hardwareID,
    required this.manufactureName,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return ListTile(
      title: Text(friendlyName),
      subtitle: Text(
          'Port: $portName\nHardware ID: $hardwareID\nManufacturer: $manufactureName'),
      leading: const Icon(Icons.usb),
    );
  }
}
