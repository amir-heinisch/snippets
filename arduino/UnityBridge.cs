/*
 * filename:	UnityBridge.cs
 * author:	Amir Heinisch <mail@amir-heinisch.de>
 * version:	16/08/2021
 */

using UnityEngine;
using UnityEngine.EventSystems;
using UnityEditor;
using System;
// Requires .NET 4.x -> Edit > Project Settings > Player > Other Settings > Api Compatibility Level*
using System.IO.Ports;
using System.Threading;
using System.Collections;

// https://docs.unity3d.com/ScriptReference/MonoBehaviour.Start.html
public class UnityBridge : MonoBehaviour
{
	static SerialPort _serialPort;

	// Use this as entry point instead of main.
	// https://docs.unity3d.com/ScriptReference/MonoBehaviour.Start.html
	void Start()
	{
		Debug.Log("Up and running!");

		// Select and configure serial port.
		_serialPort = new SerialPort();
		_serialPort.PortName = "/dev/ttyUSB0";
		_serialPort.BaudRate = 9600;

		// 8-N-1 is arduino default
		// - https://en.wikipedia.org/wiki/8-N-1
		// - https://www.arduino.cc/reference/de/language/functions/communication/serial/begin/
		_serialPort.DataBits = 8;
		_serialPort.Parity = 0;
		_serialPort.StopBits = StopBits.One;

		// Set some timeouts.
		_serialPort.ReadTimeout = 1500;
		_serialPort.WriteTimeout = 1500;

		_serialPort.Open();
		Debug.Log("Serial port opened");

	}

	void Update() {
		try
		{
			// Read message from serial port.
			Debug.Log("Serial message: " + _serialPort.ReadLine());

			// https://docs.unity3d.com/ScriptReference/Input.GetMouseButton.html
			if (Input.GetMouseButton(0))
			{
				Debug.Log("Button press detected. Send message..");
				_serialPort.WriteLine("IS ack");
			}
		}
		catch (TimeoutException) {
			// Debug.Log("TimeoutException..go on!");
		}
	}

	void OnDisable() {
		Debug.Log("Finally close serial port.");
		_serialPort.Close();
	}
}
