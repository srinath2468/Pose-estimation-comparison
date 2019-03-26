// Auto-generated. Do not edit!

// (in-package visp_hand2eye_calibration.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;

//-----------------------------------------------------------

class getposesFeedback {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.poses_sent = null;
    }
    else {
      if (initObj.hasOwnProperty('poses_sent')) {
        this.poses_sent = initObj.poses_sent
      }
      else {
        this.poses_sent = false;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type getposesFeedback
    // Serialize message field [poses_sent]
    bufferOffset = _serializer.bool(obj.poses_sent, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type getposesFeedback
    let len;
    let data = new getposesFeedback(null);
    // Deserialize message field [poses_sent]
    data.poses_sent = _deserializer.bool(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 1;
  }

  static datatype() {
    // Returns string type for a message object
    return 'visp_hand2eye_calibration/getposesFeedback';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'f555082d98a684b739b7f9ebba06a141';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    # ====== DO NOT MODIFY! AUTOGENERATED FROM AN ACTION DEFINITION ======
    # Define a feedback message
    bool poses_sent
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new getposesFeedback(null);
    if (msg.poses_sent !== undefined) {
      resolved.poses_sent = msg.poses_sent;
    }
    else {
      resolved.poses_sent = false
    }

    return resolved;
    }
};

module.exports = getposesFeedback;
