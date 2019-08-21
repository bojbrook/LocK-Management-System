package com.cmpe123.lock_management_system.ucsclocks;

/**
 * Created by bowenbrooks on 5/10/18.
 */

public class Room {
    private String roomNumber;
    private int Capacity;

    public String getRoomNumber() {
        return roomNumber;
    }

    public int getCapacity() {
        return Capacity;
    }

    public void setRoomNumber(String roomNumber) {

        this.roomNumber = roomNumber;
    }

    public void setCapacity(int capacity) {
        Capacity = capacity;
    }

    @Override
    public String toString(){
        return "Room Number " + roomNumber + " Capacity " + Capacity;
    }

}
