function list_samples(frame, chirpno, range)
 x = frame.oRXData{:};
 y = x.mData(chirpno,range);
 disp(sprintf('chirp %d sample subset',chirpno-1));
 disp(y);
end
