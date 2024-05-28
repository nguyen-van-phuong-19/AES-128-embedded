import 'dart:convert';
import 'dart:typed_data';

import 'package:encrypt/encrypt.dart' as encrypt;
import 'package:flutter/material.dart';
import 'package:serial_port_win32/serial_port_win32.dart';
import 'package:windown_embedded_app/component/custom_input.dart';
import 'package:windown_embedded_app/component/device_item.dart';

class MyHomePage extends StatefulWidget {
  const MyHomePage({super.key, required this.title});

  final String title;

  @override
  State<MyHomePage> createState() => _MyHomePageState();
}

class _MyHomePageState extends State<MyHomePage> {
  var ports = <String>[];
  late SerialPort _serialPort;
  String data = '';
  String portSelect = '';
  String keyAES_128 = 'uwkeucarscobvydv';
  String plaintext = '';
  String ciphertext = '';
  String ciphertext1 = '';
  late List<PortInfo> portInfoLists;
  late TextEditingController _controllerCOM;
  late TextEditingController _controllerKey;
  late TextEditingController _controllerPlaintext;
  bool isConnect = false;
  late encrypt.Encrypter encryter;
  late encrypt.Key keyEncryter;
  bool isTrue = false;
  bool isSend = false;

  @override
  void initState() {
    super.initState();
    portInfoLists = SerialPort.getPortsWithFullMessages();
    ports = SerialPort.getAvailablePorts();
    _controllerCOM = TextEditingController();
    _controllerKey = TextEditingController();
    _controllerPlaintext = TextEditingController();
    keyEncryter = encrypt.Key.fromUtf8(keyAES_128);
    encryter = encrypt.Encrypter(
        encrypt.AES(keyEncryter, mode: encrypt.AESMode.ecb, padding: null));
    print(portInfoLists);
    print(ports);
  }

  void setAgainKey(String key) {
    setState(() {
      keyAES_128 = key;
      keyEncryter = encrypt.Key.fromUtf8(keyAES_128);
      encryter = encrypt.Encrypter(encrypt.AES(keyEncryter, padding: null));
    });
  }

  void _getPortsAndOpen() {
    if (isConnect != true) {
      if (ports.isNotEmpty) {
        _serialPort = SerialPort(portSelect,
            BaudRate: 9600,
            openNow: false,
            ByteSize: 8,
            ReadIntervalTimeout: 1,
            ReadTotalTimeoutConstant: 2);
        _serialPort.open();
        print("connected uart!!!");
        setState(() {
          isConnect = true;
        });
        _serialPort.readBytesOnListen(16, (value) {
          print(DateTime.now());
          print(value);
          setState(() {
            ciphertext = base64Encode(Uint8List.fromList(value));
            // data = String.fromCharCodes(value);
          });
          if (ciphertext == ciphertext1) {
            isTrue = true;
          } else {
            isTrue = false;
          }
        });
      }
    }
    if (!_serialPort.isOpened) {
      setState(() {
        isConnect = false;
      });
    }
  }

  @override
  void dispose() {
    super.dispose();
    _serialPort.close();
    _controllerCOM.dispose();
    _controllerKey.dispose();
    _controllerPlaintext.dispose();
  }

  void _send(String dataSend) async {
    if (_serialPort.isOpened) {
      print(_serialPort.writeBytesFromString(dataSend));
    }
    String text_c = encryter.encrypt(dataSend).base64;
    print(encryter.encrypt(dataSend).bytes);
    print(text_c);
    // print(await _serialPort.readBytesUntil(Uint8List.fromList("\n".codeUnits)));
    isSend = true;
    setState(() {
      ciphertext1 = text_c;
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: const Color.fromARGB(255, 202, 199, 199),
      body: Padding(
        padding: const EdgeInsets.symmetric(horizontal: 8.0, vertical: 8.0),
        child: Row(
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          children: [
            Column(
              mainAxisAlignment: MainAxisAlignment.start,
              mainAxisSize: MainAxisSize.max,
              children: [
                Row(
                  children: [
                    CustomInput(
                        lableText: "Enter COM port",
                        widthSB: MediaQuery.of(context).size.width * 0.56,
                        controller: _controllerCOM),
                    const SizedBox(
                      width: 24,
                    ),
                    ElevatedButton(
                      onPressed: () {
                        setState(() {
                          portSelect = _controllerCOM.text;
                        });
                        _getPortsAndOpen();
                      },
                      child: const Padding(
                        padding: EdgeInsets.all(8.0),
                        child: Text("Connect"),
                      ),
                    ),
                  ],
                ),
                const SizedBox(
                  height: 20,
                ),
                Row(
                  children: [
                    CustomInput(
                        lableText:
                            "Enter Key (or the default key is: $keyAES_128)",
                        widthSB: MediaQuery.of(context).size.width * 0.56,
                        controller: _controllerKey),
                    const SizedBox(
                      width: 24,
                    ),
                    ElevatedButton(
                      onPressed: () {
                        setAgainKey(_controllerKey.text);
                      },
                      child: const Padding(
                        padding: EdgeInsets.all(8.0),
                        child: Text("Set key"),
                      ),
                    ),
                  ],
                ),
                const SizedBox(
                  height: 20,
                ),
                Row(
                  children: [
                    CustomInput(
                        lableText: "Enter Plaintext",
                        widthSB: MediaQuery.of(context).size.width * 0.56,
                        controller: _controllerPlaintext),
                    const SizedBox(
                      width: 24,
                    ),
                    ElevatedButton(
                      onPressed: () {
                        setState(() {
                          plaintext = _controllerPlaintext.text;
                        });
                        _send(plaintext);
                      },
                      child: const Padding(
                        padding: EdgeInsets.all(8.0),
                        child: Text("Send"),
                      ),
                    ),
                  ],
                ),
                const SizedBox(
                  height: 20,
                ),
                isSend
                    ? Container(
                        decoration: BoxDecoration(
                          color: const Color.fromARGB(255, 231, 221, 221),
                          borderRadius: BorderRadius.circular(12),
                        ),
                        child: Padding(
                          padding: const EdgeInsets.all(24.0),
                          child: SizedBox(
                            width: MediaQuery.of(context).size.width * 0.56,
                            child: Row(
                              children: [
                                Text(
                                  "The ciphertext received from stm32 is $ciphertext \nand \nthe ciphertext encrypted on the application is $ciphertext1",
                                  style: const TextStyle(
                                    fontSize: 16,
                                    fontWeight: FontWeight.w600,
                                  ),
                                ),
                                const SizedBox(
                                  width: 10,
                                ),
                                Icon(
                                  isTrue
                                      ? Icons.check
                                      : Icons.radio_button_unchecked,
                                  color: isTrue ? Colors.green : Colors.red,
                                )
                              ],
                            ),
                          ),
                        ),
                      )
                    : const SizedBox(
                        height: 20,
                      ),
              ],
            ),
            Column(
              children: [
                const Text(
                  "List of connection ports",
                  style: TextStyle(
                    fontWeight: FontWeight.w600,
                    fontSize: 25,
                  ),
                ),
                Column(
                  mainAxisAlignment: MainAxisAlignment.start,
                  mainAxisSize: MainAxisSize.max,
                  children: List.generate(portInfoLists.length, (index) {
                    return Padding(
                      padding: EdgeInsets.all(
                          MediaQuery.of(context).size.width * 0.02),
                      child: Container(
                        decoration: const BoxDecoration(
                            color: Color.fromARGB(255, 150, 187, 206),
                            borderRadius: BorderRadius.all(Radius.circular(8))),
                        width: MediaQuery.of(context).size.width * 0.24,
                        child: DeviceItem(
                            portName: portInfoLists.elementAt(index).portName,
                            friendlyName:
                                portInfoLists.elementAt(index).friendlyName,
                            hardwareID:
                                portInfoLists.elementAt(index).hardwareID,
                            manufactureName:
                                portInfoLists.elementAt(index).manufactureName),
                      ),
                    );
                  }),
                ),
                ElevatedButton(
                  onPressed: () {
                    setState(() {
                      portInfoLists = SerialPort.getPortsWithFullMessages();
                      ports = SerialPort.getAvailablePorts();
                    });
                    print(portInfoLists);
                    print(ports);
                  },
                  child: const Text("Reload"),
                )
              ],
            )
          ],
        ),
      ),
    );
  }
}
