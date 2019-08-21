package com.cmpe123.lock_management_system.ucsclocks;

/**
 * Created by bowenbrooks on 5/11/18.
 */

public class RoomLog {
    private String roomNumber;
    private String enterTime;
    private String exitTime;
    private int deltaHour;
    private int deltaMin;

    public int getDeltaHour() {
        return deltaHour;
    }

    public void setDeltaHour(int deltaHour) {
        this.deltaHour = deltaHour;
    }

    public int getDeltaMin() {
        return deltaMin;
    }

    public void setDeltaMin(int deltaMin) {
        this.deltaMin = deltaMin;
    }

    public String getEnterTime() {
        return enterTime;
    }

    public void setEnterTime(String enterTime) {
        this.enterTime = enterTime;
    }

    public String getExitTime() {
        return exitTime;
    }

    public void setExitTime(String exitTime) {
        this.exitTime = exitTime;
    }

    public String getRoomNumber() {
        return roomNumber;
    }

    public void setRoomNumber(String roomNumber) {
        this.roomNumber = roomNumber;
    }
}
