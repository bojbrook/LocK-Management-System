package com.cmpe123.lock_management_system.ucsclocks;

import android.support.v7.widget.CardView;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.util.List;

/**
 * Created by bowenbrooks on 5/11/18.
 */

public class logViewAdapter extends RecyclerView.Adapter<logViewAdapter.ViewHolder> {
    private static final String TAG = "MyAdapter";
    private List<RoomLog> mDataset;


    // Provide a suitable constructor (depends on the kind of dataset)
    public logViewAdapter(List<RoomLog> myDataset) {
        this.mDataset = myDataset;
    }
    @Override
    public logViewAdapter.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        // create a new view
        View v = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.log_view_layout, parent, false);
        logViewAdapter.ViewHolder vh = new logViewAdapter.ViewHolder(v);
        return vh;
    }

    @Override
    public void onBindViewHolder(logViewAdapter.ViewHolder holder, int position) {
        RoomLog log = mDataset.get(position);
        holder.mTV_EnterTime.setText(getTime(log.getEnterTime()));
        if(log.getExitTime() != null)
            holder.mTV_ExitTime.setText(getTime(log.getExitTime()));

        holder.mTV_Date.setText(getDate(log.getEnterTime()));
        if(position > 0){
            RoomLog previous = mDataset.get(position-1);
            String previousDate = getDate(previous.getEnterTime());
            String currentDate = getDate(log.getEnterTime());
            if(currentDate.equals(previousDate)){
                holder.mTV_Date.setVisibility(View.GONE);
            }
        }

        if(getTime(log.getExitTime()) != "") {
            String deltaString = "Hours " + log.getDeltaHour() + " Minute " + log.getDeltaMin();
            holder.mTV_DeltaTime.setText(deltaString);
        }else{
            holder.mTV_DeltaTime.setVisibility(View.GONE);
        }
    }

    private String getTime(String Date){
        int index = Date.indexOf(" ");
        if(index > 0)
            return Date.substring(0,index);
        else
            return "";
    }

    private String getDate(String Date){
        int index = Date.indexOf(" ");
        return Date.substring(index);
    }


    @Override
    public int getItemCount() {
        return mDataset.size();
    }



    public static class ViewHolder extends RecyclerView.ViewHolder {
        // each data item is just a string in this case
        public TextView mTV_EnterTime;
        public TextView mTV_ExitTime;
        public TextView mTV_DeltaTime;
        public TextView mTV_Date;
        public ViewHolder(View v) {
            super(v);
            mTV_EnterTime = v.findViewById(R.id.mTV_EnterTime);
            mTV_ExitTime = v.findViewById(R.id.mTV_ExitTime);
            mTV_DeltaTime = v.findViewById(R.id.mTV_TotalTime);
            mTV_Date = v.findViewById(R.id.mTV_Date);

        }
    }
}
