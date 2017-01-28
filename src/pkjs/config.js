// Author: Ed Dam

module.exports = [
  {
    "type": "heading",
    "defaultValue": "World v1.0"
  },
  {
    "type": "text",
    "defaultValue": "by Edward Dam"
  },
  { "type": "section", "items": [
    { "type": "heading", "defaultValue": "Time" },
    { "type": "text", "defaultValue": "Please Choose 24 Hours or 12 Hours" },
    { "type": "radiogroup", "messageKey": "hours_24", "options": [
      { "label": "24 Hours", "value": "24hours" },
      { "label": "12 Hours", "value": "12hours" } ],
    "defaultValue": "24hours" } ]
  },
  { "type": "section", "items": [
    { "type": "heading", "defaultValue": "Date" },
    { "type": "text", "defaultValue": "Please Choose Date Order" },
    { "type": "radiogroup", "messageKey": "date_order", "options": [
      { "label": "Month - Day", "value": "month-day" },
      { "label": "Day - Month", "value": "day-month" } ],
    "defaultValue": "month-day" } ]
  },
  { "type": "section", "items": [
    { "type": "heading", "defaultValue": "Temperature" },
    { "type": "text", "defaultValue": "Please Choose Fahrenheit or Celsius" },
    { "type": "radiogroup", "messageKey": "temp_degrees", "options": [
      { "label": "Fahrenheit °F", "value": "fahrenheit" },
      { "label": "Celsius °C", "value": "celsius" } ],
    "defaultValue": "fahrenheit" } ]
  },
  {
    "type": "text",
    "defaultValue": "Thank you for using my watchface."
  },
  {
    "type": "submit",
    "defaultValue": "Submit"
  }
];
