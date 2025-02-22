// Import the functions you need from the SDKs
import { initializeApp } from "https://www.gstatic.com/firebasejs/11.3.1/firebase-app.js";
import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/11.3.1/firebase-database.js";

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyBbM0alNX8Vtl8qo9rA4UlR1VV5mBKkVrE",
  authDomain: "iot-water-tank-level-project.firebaseapp.com",
  databaseURL: "https://iot-water-tank-level-project-default-rtdb.europe-west1.firebasedatabase.app",
  projectId: "iot-water-tank-level-project",
  storageBucket: "iot-water-tank-level-project.firebasestorage.app",
  messagingSenderId: "1073563069923",
  appId: "1:1073563069923:web:f385fa92333d05303a04d6"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

// Function to update water level
function updateWaterLevel(level) {
  const waterLevelElement = document.getElementById("waterLevel");
  waterLevelElement.textContent = `${level.toFixed(2)} cm`;
}

// Function to update tank status
function updateTankStatus(status) {
  const tankStatusElement = document.getElementById("tankStatus");
  tankStatusElement.textContent = status;
  //updateDataContainerColor(status); // Update background color based on status
}

// Function to update .data-container background color based on status
function updateDataContainerColor(status) {
  const dataContainer = document.querySelector(".data-container");

  switch (status.toLowerCase()) {
    case "full":
      dataContainer.style.backgroundColor = "#99ccff"; // Svijetloplava - označava pun rezervoar
      break;
    case "half-full":
      dataContainer.style.backgroundColor = "#66b3ff"; // Srednje svijetla plava - pola puna
      break;
    case "low":
      dataContainer.style.backgroundColor = "#3399ff"; // Tamnija plava - nizak nivo
      break;
    case "empty":
      dataContainer.style.backgroundColor = "#ffcccc"; // Blago crvenkasta - alarmantno stanje
      break;
    default:
      dataContainer.style.backgroundColor = "#E6F7FF"; // Podrazumijevana boja
  }
}

// Listen for changes in the "water_level" data
const waterLevelRef = ref(database, "water_level");
onValue(waterLevelRef, (snapshot) => {
  const level = snapshot.val();
  if (level !== null) {
    updateWaterLevel(level);
  }
});

// Listen for changes in the "water_status" data
const waterStatusRef = ref(database, "water_status");
onValue(waterStatusRef, (snapshot) => {
  const status = snapshot.val();
  if (status !== null) {
    updateTankStatus(status);
  }
});

console.log("Firebase listeners initialized.");
