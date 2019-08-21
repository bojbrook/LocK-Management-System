package com.cmpe123.lock_management_system.ucsclocks;

import android.support.v7.widget.CardView;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by bowenbrooks on 5/10/18.
 */

public class MyAdapter extends RecyclerView.Adapter<MyAdapter.ViewHolder> {
    private static final String TAG = "MyAdapter";
    private List<Room> mDataset;

    //declare interface
    private OnItemClicked onClick;

    //make interface like this
    public interface OnItemClicked {
        void onItemClick(int position);
    }

    // Provide a suitable constructor (depends on the kind of dataset)
    public MyAdapter(List<Room> myDataset) {
        mDataset = myDataset;
    }

    public static class ViewHolder extends RecyclerView.ViewHolder {
        // each data item is just a string in this case
        public TextView mTV_RoomNumber;
        public TextView mTV_Capacity;
        public CardView mCardView;
        public ViewHolder(View v) {
            super(v);
            mTV_RoomNumber = v.findViewById(R.id.mTV_RoomNumber);
            mTV_Capacity = v.findViewById(R.id.mTV_RoomCapacity);
            mCardView = v.findViewById(R.id.card_view);
        }
    }


    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        // create a new view
        View v = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.recycle_view_layout, parent, false);
        ViewHolder vh = new ViewHolder(v);
        return vh;
    }

    @Override
    public void onBindViewHolder(MyAdapter.ViewHolder holder, final int position) {
        holder.mTV_RoomNumber.setText(mDataset.get(position).getRoomNumber());
        holder.mTV_Capacity.setText(""+mDataset.get(position).getCapacity());

        holder.mCardView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                onClick.onItemClick(position);
            }
        });


    }

    @Override
    public int getItemCount() {
        return mDataset.size();
    }


    public void setOnClick(OnItemClicked onClick)
    {
        this.onClick=onClick;
    }


}
