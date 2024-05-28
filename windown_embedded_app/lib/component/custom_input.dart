import 'package:flutter/material.dart';

class CustomInput extends StatefulWidget {
  const CustomInput({
    super.key,
    required this.lableText,
    required this.widthSB,
    required this.controller,
  });

  final String lableText;
  final double widthSB;
  final TextEditingController controller;
  @override
  State<CustomInput> createState() => _CustomInputState();
}

class _CustomInputState extends State<CustomInput> {
  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: widget.widthSB,
      child: TextFormField(
        controller: widget.controller,
        maxLength: 32,
        decoration: InputDecoration(
          labelText: widget.lableText,
          labelStyle: const TextStyle(
            fontSize: 16,
            fontWeight: FontWeight.w500,
          ),
          border: const UnderlineInputBorder(),
        ),
        maxLines: 1,
        onSaved: (value) {},
      ),
    );
  }
}
